#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>
#include <limits.h>

using namespace std;

#define SIZE 20

// Function to find the minimum value in a given array section
int find_min(int arr[], int start, int end) {
    int min_val = INT_MAX;
    for (int i = start; i < end; ++i) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
    }
    return min_val;
}

int main() {
    srand(time(0));  // Seed for random number generator

    int arr[SIZE];

    // Fill the array with random numbers between 1 and 100
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % 100 + 1;
    }

    // Print the array
    cout << "Array: ";
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process: Find minimum in the second half of the array
        close(pipe_fd[0]);  // Close reading end of the pipe

        int child_min = find_min(arr, SIZE / 2, SIZE);
        cout << "Child Process (PID: " << getpid() << ") found min: " << child_min << endl;

        // Send the minimum value to the parent through the pipe
        write(pipe_fd[1], &child_min, sizeof(child_min));
        close(pipe_fd[1]);  // Close writing end
        exit(0);
    } else {
        // Parent process: Find minimum in the first half of the array
        close(pipe_fd[1]);  // Close writing end of the pipe

        int parent_min = find_min(arr, 0, SIZE / 2);
        cout << "Parent Process (PID: " << getpid() << ") found min: " << parent_min << endl;

        // Wait for child to finish
        wait(NULL);

        // Read the child's minimum value from the pipe
        int child_min;
        read(pipe_fd[0], &child_min, sizeof(child_min));
        close(pipe_fd[0]);  // Close reading end

        // Find overall minimum
        int overall_min = min(parent_min, child_min);
        cout << "Overall minimum value in the array: " << overall_min << endl;
    }

    return 0;
}
