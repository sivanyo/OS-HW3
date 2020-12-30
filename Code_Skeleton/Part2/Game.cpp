#include "Game.hpp"
#include "utils.hpp"

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
    std::cout << "in init game" << std::endl;
    // generating a vector with all of the lines from the file
    vector<string> lines;
    lines = utils::read_lines(filename);
    height = lines.size();

    m_thread_num = min(m_thread_num, height);

    // go over each line in lines
    // split it into separate strings where each one is a number
    // insert the converted numbers into current and next
    for (const string &line : lines) {
        vector<string> splitLine = utils::split(line, DEF_MAT_DELIMITER);
        // Now we need to go over the numbers in splitLine and insert to the array
        vector<int> splitNumberLine;
        splitNumberLine.reserve(splitLine.size());
        for (const string &num : splitLine) {
            splitNumberLine.push_back(stoi(num));
        }
        current.push_back(splitNumberLine);
        next.push_back(splitNumberLine);
    }
    width = current[0].size();
    // TODO: finish this when adding threads
    // Create threads
    // Start the threads
    // Testing of your implementation will presume all threads are started here
    //_print_internal(current);
}

void Game::_print_internal(field_mat current) {
    for (int i = 0; i < current.size(); ++i) {
        for (int j = 0; j < current[0].size(); ++j) {
            std::cout << current[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void Game::_step(uint curr_gen) {
    // TODO: complete after using threads
    // Push jobs to queue
    // Wait for the workers to finish calculating
    // Swap pointers between current and next field
    // iterate over the current field, and update the values in the next field

}

neighboors Game::calculate_neighbors(field_mat field, int i, int j) {
    int height = field.size();
    int width = field[0].size();
    neighboors env;
    for (int k = 0; k < 7; ++k) {
        env.neighborConc.push_back(0);
    }
    for (int k = i - 1; k <= i + 1; ++k) {
        for (int l = j - 1; l <= j + 1; ++l) {
            if (k == i && l == j) {
                continue;
            }
            if (is_legal_neighbor(k, l, height, width) && field[k][l] != 0) {
                env.numAlive++;
                env.neighborConc[field[k][l] % 7]++;
            }
        }
    }
    return env;
}

// This function will return the index of the most dominant species in the
// neighborhood
int Game::find_dominant_species(neighboors env) {
    int maxIndex = 0;
    int maxVal = 0;

    for (int i = 0; i < env.neighborConc.size(); ++i) {
        int tempVal = env.neighborConc[i] * i + 1;
        if (tempVal > maxVal) {
            maxIndex = i;
        }
    }
    return maxIndex;
}

bool Game::is_legal_neighbor(int i, int j, int height, int width) {
    return i >= 0 && i < height && j >= 0 && j < width;
}

void Game::_destroy_game() {
    // TODO: complete after using threads
    // Destroys board and frees all threads and resources
    // Not implemented in the Game's destructor for testing purposes.
    // Testing of your implementation will presume all threads are joined here
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
                if (current[i][j] > 0)
                    cout << colors[current[i][j] % 7] << u8"█" << RESET;
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
                                 print_on(params.print_on), filename(params.filename), height(0), width(0), current(), next() {

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


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

	    cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/



