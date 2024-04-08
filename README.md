# gcal-cli

Google Calendar CLI implemented in C++

## Build the Project

```sh
mkdir build
cd build
cmake ..

cmake --build .
```

## Config Setting

1. Download your app credential generated from Google console.
2. Place the credential under the project folder and named it as `credential.json`.
3. Execute `gcal-cli config set` and follow the instructions.

### Calendar / Task List Config
```json
{
    "calendar_list": [
        {
            "name": "Personal",
            "id": "your_calendar_id"
        },
        {
            "name": "Work",
            "id": "your_calendar_id"
        }
    ],
    "task_list": [
        {
            "name": "Personal",
            "id": "your_task_list_id"
        },
        {
            "name": "Work",
            "id": "your_task_list_id"
        }
    ]
}
```
## Supported Commands

### config
- show: show the current configuration(ALL)
- set
    - --calendar, -c: let user choose which calendars to use for fetching events
    - --timezone, -z KEYWORD: set the timezone  
    list of timezones: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones and let user choose which timezone to use. KEYWORD is the substring of the timezone used to search the timezone list. KEYWORD is case-insensitive and default to "".
    - --task, -t: let user set the task list
### event
- ls
    - -a,--days-after INT         Days after today to include in the task list
    - -b,--days-before INT        Days before today to include in the task list
    - --keyword, -k: search by title containing given keyword
- add
- rm
- edit

### task
- ls
    - -a,--days-after INT         Days after today to include in the task list
    - -b,--days-before INT        Days before today to include in the task list
    - -c,--show-completed BOOLEAN Whether to show completed tasks. Default is true.
- add
    - -t,--title TEXT             Title of the task
    - -d,--date TEXT              Due date of the task. The format should be YYYY-MM-DD.
- edit
    - -a,--days-after INT         Days after today to include in the task list
    - -b,--days-before INT        Days before today to include in the task list
