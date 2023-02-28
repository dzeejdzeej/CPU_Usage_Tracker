# CPU_Usage_Tracker
Multithread project in C to track CPU usage.

Five threads:
1. Reader  : Reads /proc/stat and send structure with proper data to another thread.
2. Analyzer: Process data from Reader and calculates CPU usage in percent for total and each
             seperate CPU, then send to another thread.
3. Printer : Prints formatted data of average CPU usage in percent, in each second.
4. Watchdog: Takes care of program workflow, if some of thread will not send the information
             through 2 seconds, Watchdog will terminate the program with proper message.
5. Logger  : Gets debug information from each thread and saves it to the file.
