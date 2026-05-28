# Диспетчер задач MercyHF AI (n8n Webhook Workflow)

Этот workflow предназначен для автоматического создания структурированных GitHub Issues для задач разработки MercyHF. Он поддерживает как ручной запуск, так и **полностью автоматический прием задач** от ИИ-агентов по протоколу HTTP (через Webhook).

---

## 🚀 Инструкция по импорту в n8n

Вы можете легко импортировать этот сценарий:

1. Откройте веб-интерфейс **n8n** (например, `http://localhost:5678`).
2. В левом меню выберите **Workflows**.
3. Создайте пустой workflow.
4. Откройте файл `Docs/n8n/MercyHF_AI_Task_Dispatcher.workflow.json` в любом текстовом редакторе, скопируйте весь его текст и нажмите **`Ctrl + V`** прямо на холсте n8n. Сценарий мгновенно развернется!
5. **Важно**: В правом верхнем углу нажмите кнопку **Active** (активировать), чтобы вебхук начал принимать внешние запросы в постоянном режиме.

---

## 🔑 Настройка учетных данных GitHub

Для работы узла **GitHub Create Issue** необходимо настроить авторизацию:

1. Дважды кликните по узлу **GitHub Create Issue** на холсте.
2. В поле **Credential for GitHub API** выберите ваше существующее подключение GitHub или создайте новое через **Create New Credential** (потребуется сгенерировать *Personal Access Token* в настройках вашего GitHub-аккаунта с правами `repo`).
3. Параметры репозитория уже настроены под ваш аккаунт:
   * **Repository Owner**: `Looney71`
   * **Repository Name**: `MercyHF1`

---

## ⚡ Автоматическая отправка задач (Webhook)

Теперь вместо ручного ввода ИИ-агенты (или любые внешние скрипты) могут отправлять HTTP-запросы напрямую на ваш локальный n8n, и задачи будут мгновенно появляться на GitHub.

### Ссылка для отправки запросов (Webhook URL):
Когда вы активируете сценарий, адрес вашего вебхука для постоянного приема задач (Production URL) будет:
`http://localhost:5678/webhook/mercyhf-task`

*(Для тестирования в режиме редактирования используйте Test URL: `http://localhost:5678/webhook-test/mercyhf-task`)*

### Формат отправляемого JSON-тела:
Для создания новой задачи нужно отправить HTTP POST-запрос с JSON следующего формата:

```json
{
  "task_title": "INDEX-0: Исправить красный свет на входе",
  "task_description": "Необходимо настроить теги ENTRY_RED_LIGHT для PointLight_11 на карте, чтобы контроллер мог управлять интенсивностью.",
  "task_type": "logic",
  "target_agent": "antigravity",
  "priority": "high",
  "level": "index-0"
}
```

### Пример отправки через curl (в терминале):
```bash
curl -X POST http://localhost:5678/webhook/mercyhf-task \
  -H "Content-Type: application/json" \
  -d "{\"task_title\":\"TEST: Новая задача от ИИ\",\"task_description\":\"Тестовое описание автоматического создания задач\",\"task_type\":\"logic\",\"target_agent\":\"antigravity\",\"priority\":\"medium\",\"level\":\"index-0\"}"
```

---

## 📝 Резервный ручной запуск и значения по умолчанию

Если вы отправите пустой запрос или нажмете кнопку запуска вручную в n8n, сценарий автоматически использует безопасные значения по умолчанию для тестовой задачи (создание визуального блокаута уровня `index-0`).

Все поля по умолчанию настроены внутри узла **Set Task Fields**:
* `task_title` — "INDEX-0: Build first playable visual blockout"
* `task_description` — "Build the first playable INDEX-0 level area in Unreal Editor..."
* `task_type` — "visual"
* `target_agent` — "antigravity"
* `priority` — "high"
* `level` — "index-0"

### Метки (Labels), создаваемые на GitHub автоматически:
* `agent:<target_agent>` (например, `agent:antigravity`)
* `type:<task_type>` (например, `type:visual`)
* `level:<level>` (например, `level:index-0`)
* `priority:<priority>` (например, `priority:high`)
