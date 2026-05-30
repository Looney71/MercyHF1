param(
    [string]$ApiBaseUrl = "https://looney.app.n8n.cloud/api/v1",
    [string]$WorkflowPath = "$PSScriptRoot\MercyHF_Autonomous_Prompt_Loop.workflow.json",
    [string]$EnvPath = "C:\Users\fores\.gemini\antigravity\scratch\.env.txt",
    [string]$StartWebhookUrl = "https://looney.app.n8n.cloud/webhook/mercyhf-loop",
    [switch]$StartLoop
)

$ErrorActionPreference = "Stop"

if (-not $env:N8N_API_KEY -and (Test-Path -LiteralPath $EnvPath)) {
    Get-Content -LiteralPath $EnvPath | ForEach-Object {
        $line = $_.Trim()
        if ($line -and -not $line.StartsWith("#") -and $line -match "^([^=]+)=(.*)$") {
            $name = $matches[1].Trim()
            $value = $matches[2].Trim().Trim('"').Trim("'")
            if ($name -in @("N8N_API_KEY", "MY_API_KEY") -and $value) {
                $env:N8N_API_KEY = $value
            }
        }
    }
}

if (-not $env:N8N_API_KEY) {
    throw "Set N8N_API_KEY in this terminal or in EnvPath. Example: `$env:N8N_API_KEY='paste-key-here'"
}

if (-not (Test-Path -LiteralPath $WorkflowPath)) {
    throw "Workflow file not found: $WorkflowPath"
}

$headers = @{
    "X-N8N-API-KEY" = $env:N8N_API_KEY
    "Accept" = "application/json"
    "Content-Type" = "application/json; charset=utf-8"
}

$workflow = Get-Content -Raw -LiteralPath $WorkflowPath | ConvertFrom-Json -Depth 100

# Public API create schema accepts workflow payload fields, not export metadata.
$payload = [ordered]@{
    name = $workflow.name
    nodes = $workflow.nodes
    connections = $workflow.connections
    settings = $workflow.settings
}

if ($workflow.staticData) {
    $payload.staticData = $workflow.staticData
}

$created = Invoke-RestMethod `
    -Method Post `
    -Uri "$ApiBaseUrl/workflows" `
    -Headers $headers `
    -Body ($payload | ConvertTo-Json -Depth 100)

if (-not $created.id) {
    throw "Workflow was created but response did not include an id."
}

$activated = Invoke-RestMethod `
    -Method Post `
    -Uri "$ApiBaseUrl/workflows/$($created.id)/activate" `
    -Headers $headers `
    -Body "{}"

[pscustomobject]@{
    WorkflowId = $created.id
    WorkflowName = $created.name
    Active = $activated.active
    Webhook = $StartWebhookUrl
} | ConvertTo-Json -Depth 10

if ($StartLoop) {
    $startPayload = @{
        loop_id = "mercyhf-main-architecture-loop"
        target_agent = "antigravity"
        level_focus = "ProjectWide"
        mode = "durable-forward-development"
        iteration = 0
        max_iterations = 24
        delay_seconds = 900
        loop_enabled = $true
        repository_owner = "Looney71"
        repository_name = "MercyHF1"
        webhook_base_url = $StartWebhookUrl
        last_result_summary = "Start from current main branch and local project rules. Continue architectural level development without broad rewrites."
    }

    $loopResponse = Invoke-RestMethod `
        -Method Post `
        -Uri $StartWebhookUrl `
        -ContentType "application/json; charset=utf-8" `
        -Body ($startPayload | ConvertTo-Json -Depth 20)

    [pscustomobject]@{
        Started = $true
        Response = $loopResponse
    } | ConvertTo-Json -Depth 10
}
