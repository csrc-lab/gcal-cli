# gcal-cli

Google Calendar CLI implemented in C++

## Build the Project

```sh
mkdir build
cd build
cmake .. -G "Unix Makefiles"

make
```

## Config Setting

1. Download your app credential generated from Google console.
2. Place the credential under the project folder and named it as `credential.json`.
3. Execute `gcal-cli config set` and follow the instructions.
