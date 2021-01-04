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
        m_gen_hist.push_back((float) std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
        print_board(NULL);
    } // generation loop
    print_board("Final Board");
    _destroy_game();
}

void Game::_init_game() {
    // generating a vector with all of the lines from the file
    vector<string> lines;
    lines = utils::read_lines(filename);
    height = lines.size();

    m_thread_num = min(m_thread_num, height);
    field_mat startingField;
    // go over each line in lines
    // split it into separate strings where each one is a number
    // insert the converted numbers into current and next
    for (const string &line : lines) {
        vector<string> splitLine = utils::split(line, DEF_MAT_DELIMITER);
        width = splitLine.size();
        // Now we need to go over the numbers in splitLine and insert to the array
        vector<int> splitNumberLine;
        splitNumberLine.reserve(splitLine.size());
        for (const string &num : splitLine) {
            splitNumberLine.push_back(stoi(num));
        }
        startingField.push_back(splitNumberLine);
    }
    current = new field_mat(startingField);
    next = new field_mat(startingField);
    //width = current[0].size();
    rowsPerThread = height / m_thread_num;

    uint startRow = 0;
    uint endRow = 0;
    for (unsigned int i = 0; i < m_thread_num; ++i) {
        startRow = i * rowsPerThread;
        if (i == m_thread_num - 1) {
            // this is the last thread, so it will run from it's start point until the end of the matrix
            endRow = height - 1;
        } else {
            endRow = startRow + rowsPerThread - 1;
        }
        auto *t = new WorkingThread(i, current, next, startRow, endRow, height, width, &m_tile_hist);
        m_threadpool.push_back(t);
        // The exercise assumes all threads are started here
        //t->start();
    }
    // TODO: finish this when adding threads
    // Create threads
    // Start the threads
    // Testing of your implementation will presume all threads are started here
    //_print_internal(current);
}

void Game::_step(uint curr_gen) {
    // TODO: complete after using threads
    // Push jobs to queue
    // Wait for the workers to finish calculating
    // Swap pointers between current and next field
    // iterate over the current field, and update the values in the next field
    for (auto &it : m_threadpool) {
        it->start();
    }

    for (auto &it : m_threadpool) {
        it->join();
    }

    for (auto &it : m_threadpool) {
        it->start();
    }

    for (auto &it : m_threadpool) {
        it->join();
    }
}

neighboors Game::calculate_neighbors(field_mat *field, int i, int j) {
    int height = (*field).size();
    int width = field[0].size();
    neighboors env;
    env.numAlive = 0;
    for (int k = 0; k < 7; ++k) {
        env.neighborConc.push_back(0);
    }
    for (int k = i - 1; k <= i + 1; ++k) {
        for (int l = j - 1; l <= j + 1; ++l) {
            if (k == i && l == j) {
                continue;
            }
            if (is_legal_neighbor(k, l, height, width) && (*field)[k][l] != 0) {
                env.numAlive++;
                env.neighborConc[(*field)[k][l] % 7]++;
            }
        }
    }
    return env;
}

// This function will return the index of the most dominant species in the
// neighborhood
int Game::find_dominant_species(neighboors env) {
    int maxIndex = -1;
    int maxVal = 0;

    // 1 is red
    // 0 is black (it's actually 7)
    for (unsigned int i = 1; i < env.neighborConc.size(); ++i) {
        int tempVal = env.neighborConc[i] * i;
        if (tempVal > maxVal) {
            maxVal = tempVal;
            maxIndex = i;
        }
    }

    int lastVal = env.neighborConc[0] * 7;
    if (lastVal > maxVal) {
        maxIndex = 7;
    }
    return maxIndex;
}

int Game::change_species_from_neighbors(neighboors env, int selfVal) {
    int sum = selfVal;
    int counter = 1;
    for (unsigned int i = 1; i < env.neighborConc.size(); ++i) {
        if (env.neighborConc[i] != 0) {
            sum += env.neighborConc[i] * i;
            counter += env.neighborConc[i];
        }
    }

    if (env.neighborConc[0] != 0) {
        sum += env.neighborConc[0] * 7;
        counter += env.neighborConc[0];
    }
    return std::round(sum / counter);
}

bool Game::is_legal_neighbor(int i, int j, int height, int width) {
    return i >= 0 && i < height && j >= 0 && j < width;
}

void Game::_destroy_game() {
    // Destroys board and frees all threads and resources
    // Not implemented in the Game's destructor for testing purposes.
    // Testing of your implementation will presume all threads are joined here
    // will need to clear the boards

    delete current;
    delete next;
    // clearing all of the threads
//    for (int i = 0; i < m_thread_num; ++i) {
//        m_threadpool[i]->join();
//        delete m_threadpool[i];
//    }
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
                                 print_on(params.print_on), filename(params.filename), height(0), width(0), rowsPerThread(0), current(), next() {

}

uint Game::thread_num() const {
    return m_thread_num;
}

const vector<float> Game::gen_hist() const {
    return m_gen_hist;
}

const vector<float> Game::tile_hist() const {
    return m_tile_hist;
}



