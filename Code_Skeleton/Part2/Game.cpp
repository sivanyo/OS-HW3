#include "Game.hpp"
#include "utils.hpp"
#include "WorkingThread.h"

using std::min;

static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

/*--------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
void Game::run() {

    _init_game(); // Starts the threads and all other variables you need
    print_board("Initial Board");
    for (uint i = 0; i < m_gen_num; ++i) {
        auto gen_start = std::chrono::system_clock::now();
        _step(i); // Iterates a single generation
        auto gen_end = std::chrono::system_clock::now();
        m_gen_hist.push_back((double) std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
        print_board(nullptr);
    } // generation loop
    print_board("Final Board");
    _destroy_game();
}

// Create game fields - Consider using utils:read_file, utils::split
// Create & Start threads
// Testing of your implementation will presume all threads are started here
void Game::_init_game() {
    // generating a vector with all of the lines from the file
    vector<string> lines;
    lines = utils::read_lines(filename);
    height = lines.size();

    m_thread_num = min(m_thread_num, height);
    int_mat startingField;
    // go over each line in lines
    // split it into separate strings where each one is a number
    // insert the converted numbers into current and next
    for (const string &line : lines) {
        vector<string> splitLine = utils::split(line, DEF_MAT_DELIMITER);
        width = splitLine.size();
        // Now we need to go over the numbers in splitLine and insert to the array
        vector<uint> splitNumberLine;
        splitNumberLine.reserve(splitLine.size());
        for (const string &num : splitLine) {
            splitNumberLine.push_back(stoi(num));
        }
        startingField.push_back(splitNumberLine);
    }
    current = new int_mat(startingField);
    next = new int_mat(startingField);
    //width = current[0].size();
    rowsPerThread = height / m_thread_num;

    for (unsigned int i = 0; i < m_thread_num; ++i) {
        auto *t = new WorkingThread(i, &jobQueue, &m_tile_time, &threadLock, &finishedJobCounter);
        m_threadpool.push_back(t);
        // The exercise assumes all threads are started here
        t->start();
    }
}

// Push jobs to queue
// Wait for the workers to finish calculating
// Swap pointers between current and next field
// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade
void Game::_step(uint curr_gen) {
    // Create phase 1 jobs
    for (unsigned int i = 0; i < m_thread_num; ++i) {
        TileJob job{};
        job.height = height;
        job.width = width;
        job.current = current;
        job.next = next;
        job.isLastGen = false;
        job.currPhase = 1;

        job.startRow = i * rowsPerThread;
        if (i == m_thread_num - 1) {
            // this is the last thread, so it will run from it's start point until the end of the matrix
            job.endRow = height - 1;
        } else {
            job.endRow = job.startRow + rowsPerThread - 1;
        }
        jobQueue.push(job);
    }

    while (finishedJobCounter != m_thread_num) {
        // Busy wait of main game thread
    }
    finishedJobCounter = 0;

    // Create phase 2 jobs
    for (unsigned int i = 0; i < m_thread_num; ++i) {
        TileJob job{};
        job.height = height;
        job.width = width;
        job.current = current;
        job.next = next;
        job.isLastGen = false;
        job.currPhase = 2;

        job.startRow = i * rowsPerThread;
        if (i == m_thread_num - 1) {
            // this is the last thread, so it will run from it's start point until the end of the matrix
            job.endRow = height - 1;
        } else {
            job.endRow = job.startRow + rowsPerThread - 1;
        }
        jobQueue.push(job);
    }

    while (finishedJobCounter != m_thread_num) {
        // Busy wait of main game thread
    }
    finishedJobCounter = 0;

    if (curr_gen == m_gen_num - 1) {
        // This is the last generation of the game, so we give all threads a job that will cause them to finish
        for (unsigned int i = 0; i < m_thread_num; ++i) {
            TileJob job{};
            job.isLastGen = true;
            jobQueue.push(job);
        }
    }
}

// Destroys board and frees all threads and resources
// Not implemented in the Game's destructor for testing purposes.
// All threads must be joined here
void Game::_destroy_game() {
    delete current;
    delete next;

    // clearing all of the threads
    for (unsigned int i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
        delete m_threadpool[i];
    }
    // Pass timing back to the general purpose vector
    for (auto & i : m_tile_time) {
        m_tile_hist.push_back(i.threadTime);
    }
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char *header) {

    if (print_on) {

        // Clear the screen, to create a running animation
        if (interactive_on)
            system("clear");

        // Print small header if needed
        if (header != NULL)
            cout << "<------------" << header << "------------>" << endl;

        cout << u8"╔" << string(u8"═") * width << u8"╗" << endl;
        for (uint i = 0; i < height; ++i) {
            cout << u8"║";
            for (uint j = 0; j < width; ++j) {
                if ((*current)[i][j] > 0)
                    cout << colors[(*current)[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * width << u8"╝" << endl;

        // Display for GEN_SLEEP_USEC micro-seconds on screen
        if (interactive_on)
            usleep(GEN_SLEEP_USEC);
    }

}

Game::Game(game_params params) : m_gen_num(params.n_gen), m_thread_num(params.n_thread), interactive_on(params.interactive_on),
                                 print_on(params.print_on), filename(params.filename), height(0), width(0), rowsPerThread(0), current(), next(),
                                 jobQueue(), finishedJobCounter(0) {
    pthread_mutex_init(&threadLock, NULL);
}

uint Game::thread_num() const {
    return m_thread_num;
}

const vector<double> Game::gen_hist() const {
    return m_gen_hist;
}

const vector<double> Game::tile_hist() const {
    return m_tile_hist;
}



