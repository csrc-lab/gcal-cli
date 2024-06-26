#include <CLI11.hpp>

#include "ConfigManager.h"
#include "GoogleEventsAPI.h"
#include "GoogleTasksAPI.h"

int main(int argc, char **argv) {
#ifdef _WIN32
    system("chcp 65001");  // Set the console to UTF-8 mode
#endif

    bool showCompleted = true;
    int daysBefore = 7;
    int daysAfter = 7;
    std::string keyword = "";

    CLI::App app{"Google Calendar CLI"};
    argv = app.ensure_utf8(argv);

    app.require_subcommand(1);
    auto help = app.add_subcommand("help")->silent();
    help->parse_complete_callback([]() { throw CLI::CallForHelp(); });

    CLI::App *configApp =
        app.add_subcommand("config", "Configure the authentication settings");
    configApp->require_subcommand(1);
    auto configShow =
        configApp->add_subcommand("show", "Show the current configuration");
    configShow->callback([]() { ConfigManager::showConfiguration(); });

    auto configSet = configApp->add_subcommand("set", "Set the configuration");
    std::string credPath;
    configSet->add_option<std::string>(
        "-f,--file", credPath,
        "Path to the credential file. To create a credential file, follow the "
        "steps to set up a Google OAuth 2.0 desktop application at "
        "https://developers.google.com/identity/protocols/oauth2/native-app. "
        "Make sure to enable both the Google Tasks API and Google Calendar API "
        "in the API Console to ensure full functionality.");
    configSet->callback(
        [&credPath]() { ConfigManager::setConfiguration(credPath); });

    auto *eventApp = app.add_subcommand("event", "Manage events");
    eventApp->require_subcommand(1);
    auto eventList = eventApp->add_subcommand("ls", "List events");
    eventList->add_option("-a,--days-after", daysAfter,
                          "Days after today to include in the event list");
    eventList->add_option("-b,--days-before", daysBefore,
                          "Days before today to include in the event list");
    eventList->add_option(
        "-k, --keyword", keyword,
        "text search terms to find events that match these terms in the "
        "following fields: summary, description, location");

    eventList->callback([&]() {
        try {
            GoogleEventsAPI googleEventsAPI = GoogleEventsAPI();
            googleEventsAPI.list(daysBefore, daysAfter, keyword);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });
    auto eventAdd = eventApp->add_subcommand("add", "Add an event");
    eventAdd->callback([]() {
        try {
            GoogleEventsAPI googleEventsAPI = GoogleEventsAPI();
            googleEventsAPI.add();
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    auto *taskApp = app.add_subcommand("task", "Manage tasks");
    taskApp->require_subcommand(1);
    auto taskList = taskApp->add_subcommand("ls", "List tasks");
    taskList->add_option("-a,--days-after", daysAfter,
                         "Days after today to include in the task list");
    taskList->add_option("-b,--days-before", daysBefore,
                         "Days before today to include in the task list");
    taskList->add_option<bool>(
        "-c,--show-completed", showCompleted,
        "Whether to show completed tasks. Default is true.");
    taskList->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.list(showCompleted, daysBefore, daysAfter);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    auto taskAdd = taskApp->add_subcommand("add", "Create tasks");
    std::string title = "";
    std::string dueDate = "";
    taskAdd->add_option("-t,--title", title, "Title of the task");
    taskAdd->add_option(
        "-d,--date", dueDate,
        "Due date of the task. The format should be YYYY-MM-DD. The time "
        "portion of the timestamp is discarded when setting the due date.");
    taskAdd->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.add(title, dueDate);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    auto taskEdit = taskApp->add_subcommand("edit", "Edit tasks");
    int daysBeforeTaskEdit = 3;
    int daysAfterTaskEdit = 3;
    taskEdit->add_option("-a,--days-after", daysAfterTaskEdit,
                         "Days after today to include in the task list");
    taskEdit->add_option("-b,--days-before", daysBeforeTaskEdit,
                         "Days before today to include in the task list");
    taskEdit->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.edit(daysBeforeTaskEdit, daysAfterTaskEdit);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}
