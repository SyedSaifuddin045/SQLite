#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

std::vector<std::string> run_script(const std::vector<std::string>& commands) {
    int in_pipe[2];  // Pipe for input to the child
    int out_pipe[2]; // Pipe for output from the child

    if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        dup2(in_pipe[0], STDIN_FILENO);  // Redirect stdin to read from in_pipe
        dup2(out_pipe[1], STDOUT_FILENO); // Redirect stdout to write to out_pipe

        // Close unused pipe ends
        close(in_pipe[1]);
        close(out_pipe[0]);

        // Execute the program
        execl("sqlite", "./db", nullptr);
        perror("execl"); // If execl fails
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        close(in_pipe[0]);  // Close unused read end
        close(out_pipe[1]); // Close unused write end

        // Write commands to the child process
        std::ostringstream input_stream;
        for (const auto& command : commands) {
            input_stream << command << "\n";
        }
        std::string input = input_stream.str();
        write(in_pipe[1], input.c_str(), input.size());
        close(in_pipe[1]); // Close the write end to signal EOF

        // Read output from the child process
        std::ostringstream output_stream;
        char buffer[1024];
        ssize_t count;
        while ((count = read(out_pipe[0], buffer, sizeof(buffer))) > 0) {
            output_stream.write(buffer, count);
        }
        close(out_pipe[0]); // Close the read end

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        // Split output into lines
        std::vector<std::string> output_lines;
        std::istringstream output_splitter(output_stream.str());
        std::string line;
        while (std::getline(output_splitter, line)) {
            output_lines.push_back(line);
        }
        return output_lines;
    }
}

TEST(DatabaseTest, InsertAndRetrieveRow) {
    std::vector<std::string> result = run_script({
        "insert 1 user1 person1@example.com",
        "select",
        ".exit",
    });

    std::vector<std::string> expected = {
        "db > Executed.",
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > ",
    };

    ASSERT_EQ(result, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
