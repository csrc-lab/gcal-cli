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

## Supported Commands

```sh
config
    show
    set
event
    ls
        --past, -p: default to display past 7 days,
        --upcoming, -u: default to display future 7 days
        --keyword, -k: search by title containing given keyword
    add
        --start
        --end
        --title
    rm
    edit
task
    ls
        -a,--days-after INT         Days after today to include in the task list
        -b,--days-before INT        Days before today to include in the task list
        -c,--show-completed BOOLEAN Whether to show completed tasks. Default is true.
    add
        -t,--title TEXT             Title of the task
        -d,--date TEXT              Due date of the task. The format should be YYYY-MM-DD.
    rm
    edit
```
