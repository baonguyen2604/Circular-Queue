# C Implementation of Concurrent Circular Queue

- Robust library implementation of a concurrent circular buffer
- Supports concurrent read access from multiple threads but limit write access to single thread using POSIX read-write lock
- Returns error with descriptive `errno` if any operations encounter errors
- The state of the queue is consistent if errors occurred
- Provide a test file `circular_queue_test`
- Build with `make`