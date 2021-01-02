//
// Created by S on 1/1/21.
//
#include <string>
#include <fstream>
#include <iostream>

# include "../Code_Skeleton/Part1/Headers.hpp"  // TODO set right dirs
# include "../Code_Skeleton/Part2/Game.hpp"  // TODO set right dirs

using namespace std;

bool compare_boards(string output, string expected) {
    ifstream output_f,expected_f;
    output_f.open(output);
    expected_f.open(expected);

    std::string string1, string2;
    int num_of_diff = 0;
    int i = 1;
    while(std::getline(expected_f, string2)) {
        std::getline(output_f, string1);
        if(string1 != string2) {
            if (!num_of_diff) {
                cout << "found the next differences:" << endl;
            }

            cout << "line " << i << ": + " << string1 << endl;
            cout << "line " << i << ": - " << string2 << endl;
            if (++num_of_diff > 222) {
                cout << "too much differences... stops the comparing" << endl;
                break;
            }
        }
        ++i;
    }
    return num_of_diff == 0;
}

void create_random_board(string board_name = "board1", int r = 100, int c = 200) {
    int row = 1 + (rand() % r);
    int col = 1 + (rand() % c);

    ofstream m_file(board_name + ".txt");

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int val = rand() % 2 == 1 ? 0 : rand() % 8;
            m_file << val;
            if (j != col-1) m_file << " ";
        }
        if (i != row-1) m_file << "\n";
    }
    m_file.close();
}

bool run_general_test(string test_name, string board_name, int gen, int thread) {
    string input_dir = "../tester0/" + board_name + ".txt";
    string output_dir = "../tester0/" + board_name + "-output.txt";  // TODO (just if you want to:) set right dirs
    string expected_dir = "../tester0/" + board_name + "-expected.txt";

    cout << "********* start running test: " << test_name << " *********" << endl;
    cout << "**** board: " << board_name << ", with " << thread << " thread(s) for " << gen << " generation(s) ****" << endl;

    struct game_params p1;
    p1.n_gen = gen;
    p1.n_thread = thread;
    p1.filename = input_dir;
    p1.interactive_on = false;
    p1.print_on = true;

    Game* g = new Game(p1);

    std::ofstream out(output_dir);
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout

    g->run();

    std::cout.rdbuf(coutbuf); //reset to standard output again
    out.close();

    if (compare_boards(expected_dir, output_dir)) {
        cout << "********* PASS " << test_name << " with success! *********" << endl << endl;
        return true;
    }

    cout << "********* FAILED " << test_name << "... sorry, maybe next time *********" << endl << endl;
    return false;
}

int main () {

    cout << "THIS TESTER IS NOT YET COMPLETED! DONT RELY ON IT 100% !" << endl;

    int test_pass = 0;
    test_pass += run_general_test("SEGEL example boards", "tiny", 1, 1);
    test_pass += run_general_test("SEGEL example boards", "tiny2", 2, 1);
    test_pass += run_general_test("SEGEL example boards", "tiny3", 10, 5);
    test_pass += run_general_test("SEGEL example boards", "small", 10, 1);
    test_pass += run_general_test("SEGEL example boards", "small2", 10, 5);
    test_pass += run_general_test("SEGEL example boards", "mid", 3, 1);
    test_pass += run_general_test("SEGEL example boards", "mid2", 10, 5);
    test_pass += run_general_test("SEGEL example boards", "big", 10, 5);
    test_pass += run_general_test("SEGEL example boards", "big2", 10, 5);

    test_pass += run_general_test("random board 1", "rb1", 15, 7);
    test_pass += run_general_test("random board 2", "rb2", 5, 10);
    test_pass += run_general_test("random board 3", "rb3", 7, 14);
    test_pass += run_general_test("random board 4", "rb4", 12, 6);
    test_pass += run_general_test("random board 5", "rb5", 22, 5);
    test_pass += run_general_test("random board 6", "rb6", 7, 24);
    test_pass += run_general_test("random board 7", "rb7", 11, 30);
    test_pass += run_general_test("random board 8", "rb8", 6, 40);

    if (test_pass == 17) {
        cout << "<<< !!! PASS ALL TESTS !!! 17 out of 17 >>>" << endl;
    }
    else {
        cout << "<<< did not pass all tests. pass only " << test_pass << " out of 17 >>>" << endl;
    }

    cout << "THIS TESTER IS NOT YET COMPLETED! DONT RELY ON IT 100% !" << endl;
}

