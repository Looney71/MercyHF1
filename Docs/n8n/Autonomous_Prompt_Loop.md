# MercyHF Autonomous Prompt Loop

This workflow creates a controlled autonomous prompting loop for MercyHF.

Import:
- `Docs/n8n/MercyHF_Autonomous_Prompt_Loop.workflow.json`

Production webhook after import and activation:
- `https://looney.app.n8n.cloud/webhook/mercyhf-loop`

Purpose:
- create one GitHub issue per loop iteration;
- rotate through durable architectural passes for `Index0_Game`, `Index1_Game`,
  `Index2_Game`, and shared systems;
- carry state through `loop_id`, `iteration`, `max_iterations`, and
  `last_result_summary`;
- wait between iterations and call itself again.

Default loop behavior:
- max iterations: `24`
- delay between prompts: `900` seconds
- target agent: `antigravity`
- first phase: `Index0_Game`

Start payload:

```json
{
  "loop_id": "mercyhf-main-architecture-loop",
  "target_agent": "antigravity",
  "level_focus": "ProjectWide",
  "mode": "durable-forward-development",
  "iteration": 0,
  "max_iterations": 24,
  "delay_seconds": 900,
  "loop_enabled": true,
  "repository_owner": "Looney71",
  "repository_name": "MercyHF1",
  "webhook_base_url": "https://looney.app.n8n.cloud/webhook/mercyhf-loop",
  "last_result_summary": "Start from current main branch and local project rules. Continue architectural level development without broad rewrites."
}
```

Stop payload:

```json
{
  "loop_id": "mercyhf-main-architecture-loop",
  "loop_enabled": false,
  "iteration": 24,
  "max_iterations": 24
}
```

Notes:
- The workflow does not add a separate manual verification stage.
- Quality requirements are embedded directly into every generated prompt.
- Agents must still report changed files, map/assets touched, design decisions,
  next task, and human blockers.
- Without an n8n API key this repository can only provide an importable workflow;
  cloud workflow replacement must be done from the n8n UI or an authenticated
  n8n API call.

Programmatic import after creating an API key:

```powershell
$env:N8N_API_KEY = "paste-key-here"
.\Docs\n8n\Import_Autonomous_Loop.ps1 -StartLoop
```

The script can also read `N8N_API_KEY` or `MY_API_KEY` from:

```text
C:\Users\fores\.gemini\antigravity\scratch\.env.txt
```

The script imports `MercyHF_Autonomous_Prompt_Loop.workflow.json`, activates it,
and optionally starts the first loop iteration.
