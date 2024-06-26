# os_sim

## Overview
`os_sim` is a C++ project simulating an operating system. The project includes several files to manage different aspects of the operating system simulation. An upcoming feature will be allowing the user to make real time changes to the CPU (as of right now, you can only use OS functions in the main function).

## Files
- `CMakeLists.txt`: CMake configuration file to build the project.
- `OpSys.cpp`: Contains the main functionality of the operating system simulation.
- `OpSys.h`: Header file for the `OpSys` class.
- `PCB.cpp`: Implementation of the Process Control Block (PCB).
- `PCB.h`: Header file for the `PCB` class.
- `utility.cpp`: Contains utility functions used by the project.
- `README.md`: Project documentation.

## Compilation Instructions

### Using CMake
1. **Install CMake**: Ensure you have CMake installed on your system. You can download it from [here](https://cmake.org/download/).
2. **Create a Build Directory and enter it**: Navigate to your project directory and create a build directory.
    ```sh
    mkdir build
    cd build
    ```
3. **Generate Build Files**: Run CMake to generate the build files.
    ```sh
    cmake ..
    ```
4. **Build the Project**: Compile the project using the generated build files.
    ```sh
    cmake --build .
    ```
5. **Run the Executable**: After building, you can run the generated executable (the name will depend on your configuration).
    ```sh
    ./os_sim
    ```

### Using g++
If you prefer using `g++` to compile the project manually, you can do the following:
1. **Compile**: Use `g++` to compile the source files.
    ```sh
    g++ -o os_sim OpSys.cpp PCB.cpp utility.cpp
    ```
2. **Run**: Execute the compiled program.
    ```sh
    ./os_sim
    ```

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author
- **umarhunter**

## Contact
For any questions or issues, please contact **umarhunter**.
