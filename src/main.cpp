#include "CLI.h"

int main(int argc, char* argv[])
{
    gitz::CLI cli(argc, argv);
    bool success = cli.execute();
    return success ? 0 : 1;
}
