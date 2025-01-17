#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

std::vector<std::string> run_script(const std::vector<std::string> &commands)
{
    int in_pipe[2];  // Pipe for input to the child
    int out_pipe[2]; // Pipe for output from the child

    if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        dup2(in_pipe[0], STDIN_FILENO);   // Redirect stdin to read from in_pipe
        dup2(out_pipe[1], STDOUT_FILENO); // Redirect stdout to write to out_pipe

        // Close unused pipe ends
        close(in_pipe[1]);
        close(out_pipe[0]);

        // Execute the program
        execl("./sqlite", "./db", "test.db", nullptr);
        perror("execl"); // If execl fails
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        close(in_pipe[0]);  // Close unused read end
        close(out_pipe[1]); // Close unused write end

        // Write commands to the child process
        std::ostringstream input_stream;
        for (const auto &command : commands)
        {
            input_stream << command << "\n";
        }
        std::string input = input_stream.str();
        write(in_pipe[1], input.c_str(), input.size());
        close(in_pipe[1]); // Close the write end to signal EOF

        // Read output from the child process
        std::ostringstream output_stream;
        char buffer[1024];
        ssize_t count;
        while ((count = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
        {
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
        while (std::getline(output_splitter, line))
        {
            output_lines.push_back(line);
        }
        return output_lines;
    }
}

class DatabaseTest : public ::testing::Test
{
protected:
    // Cleanup before each test
    void SetUp() override
    {
        // Delete the test.db file before each test
        remove("test.db");
    }

    static void TearDownTestSuite()
    {
        // Delete the test.db file after all tests have finished
        remove("test.db");
    }
};

TEST_F(DatabaseTest, InsertAndRetrieveRow)
{
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

TEST_F(DatabaseTest, AllowsInsertingStringsOfMaxLength)
{
    std::string long_username(32, 'a');
    std::string long_email(255, 'a');

    std::vector<std::string> script = {
        "insert 1 " + long_username + " " + long_email,
        "select",
        ".exit"};

    std::vector<std::string> result = run_script(script);

    std::vector<std::string> expected = {
        "db > Executed.",
        "db > (1, " + long_username + ", " + long_email + ")",
        "Executed.",
        "db > "};

    EXPECT_EQ(result, expected);
}

TEST_F(DatabaseTest, PrintsErrorMessageIfStringsAreTooLong)
{
    std::string long_username(33, 'a');
    std::string long_email(256, 'a');

    std::vector<std::string> script = {
        "insert 1 " + long_username + " " + long_email,
        "select",
        ".exit"};

    std::vector<std::string> result = run_script(script);

    std::vector<std::string> expected = {
        "db > String is too long.",
        "db > Executed.",
        "db > "};

    EXPECT_EQ(result, expected);
}

TEST_F(DatabaseTest, PrintsErrorMessageIfIdIsNegative)
{
    std::vector<std::string> script = {
        "insert -1 cstack foo@bar.com",
        "select",
        ".exit"};
    std::vector<std::string> result = run_script(script);
    std::vector<std::string> expected = {
        "db > ID must be positive.",
        "db > Executed.",
        "db > "};
    EXPECT_EQ(result, expected);
}

TEST_F(DatabaseTest, KeepsDataAfterClosingConnection)
{
    std::vector<std::string> result1 = run_script({
        "insert 1 user1 person1@example.com",
        ".exit",
    });

    std::vector<std::string> expected1 = {
        "db > Executed.",
        "db > "};
    EXPECT_EQ(result1, expected1);

    std::vector<std::string> result2 = run_script({
        "select",
        ".exit",
    });

    std::vector<std::string> expected2 = {
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > "};
    EXPECT_EQ(result2, expected2);
}

TEST_F(DatabaseTest, PrintsConstants)
{
    std::vector<std::string> script = {
        ".constants",
        ".exit",
    };

    std::vector<std::string> result = run_script(script);

    std::vector<std::string> expected = {
        "db > Constants:",
        "ROW_SIZE: 293",
        "COMMON_NODE_HEADER_SIZE: 6",
        "LEAF_NODE_HEADER_SIZE: 10",
        "LEAF_NODE_CELL_SIZE: 297",
        "LEAF_NODE_SPACE_FOR_CELLS: 4086",
        "LEAF_NODE_MAX_CELLS: 13",
        "db > ",
    };

    EXPECT_EQ(result, expected);
}

TEST_F(DatabaseTest, PrintsOneNodeBtreeStructure)
{
    // Create the script for inserting rows and viewing the B-tree structure
    std::vector<std::string> script = {
        "insert 3 user3 person3@example.com",
        "insert 1 user1 person1@example.com",
        "insert 2 user2 person2@example.com",
        ".btree",
        ".exit",
    };

    // Execute the script
    std::vector<std::string> result = run_script(script);

    // Expected output
    std::vector<std::string> expected = {
        "db > Executed.",
        "db > Executed.",
        "db > Executed.",
        "db > Tree:",
        "leaf (size 3)",
        "  - 0 : 1",
        "  - 1 : 2",
        "  - 2 : 3",
        "db > ",
    };

    // Compare the actual result with the expected output
    EXPECT_EQ(result, expected);
}

TEST(DatabaseTests, PrintsErrorMessageForDuplicateId)
{
    std::vector<std::string> script = {
        "insert 1 user1 person1@example.com",
        "insert 1 user1 person1@example.com",
        "select",
        ".exit"};

    std::vector<std::string> expected_output = {
        "db > Executed.",
        "db > Error: Duplicate key.",
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > "};

    std::vector<std::string> result = run_script(script);

    EXPECT_EQ(result, expected_output);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
