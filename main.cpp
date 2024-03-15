#include <CLI11.hpp>

/*
Supported command
config
    show
    set
event
    list
    add
    delete
    update
task
    list
    add
    delete
    update
*/

int main(int argc, char **argv) {
    CLI::App app{"Google Calendar CLI"};
    argv = app.ensure_utf8(argv);

    std::string filename = "default";
    app.require_subcommand(1);
    auto help = app.add_subcommand("help")->silent();
    help->parse_complete_callback([]() { throw CLI::CallForHelp(); });

    CLI::App *configApp =
        app.add_subcommand("config", "Configure the authentication settings");
    configApp->require_subcommand(1);
    auto configShow =
        configApp->add_subcommand("show", "Show the current configuration");
    configShow->callback(
        []() { std::cout << "Show the current configuration" << std::endl; });
    auto configSet =
        configApp->add_subcommand("set", "Set the configuration");
    configSet->callback([]() { std::cout << "Set the configuration" << std::endl; });

    app.add_subcommand("event", "Manage events");
    app.add_subcommand("task", "Manage tasks");

    CLI11_PARSE(app, argc, argv);

    return 0;
}
