
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// using namespace std;

class Tile{
private:
    int row_index;
    int col_index;
    char color;
    bool visited;
    std::pair<int, int> from;
    
public:
    Tile()
    :row_index(-1), col_index(-1), color(' '), visited(0), from(0,0){};
    
    Tile(int row_in, int col_in, char color_in) :
    row_index(row_in), col_index(col_in), color(color_in){
        visited = false;
    };
    
    void operator = (const Tile &rhs){
        row_index = rhs.row_index;
        col_index = rhs.col_index;
        color = rhs.color;
    }
    bool operator == (Tile &rhs){
        return ((row_index == rhs.row_index)
                && (col_index == rhs.col_index)
                && (color == rhs.color));
    }
    int row(){
        return row_index;
    }
    int col(){
        return col_index;
    }
    char get_color(){
        return color;
    }
    void visit(){
        visited = true;
    }
    bool is_visited(){
        return visited;
    }
    bool is_blocked(){
        return color != '0' ? true : false;
    }
    bool is_boundary(){
        return color == '%' ? true : false;
    }
    void mark_as_path(){
        color = '*';
    }
    void came_from(Tile &origin){
        from.first = origin.row();
        from.second = origin.col();
    }
    std::pair<int, int> get_origin(){
        return from;
    }
    bool is_valid_to_add_to_queue(){
        return (visited == false) && (color == '0') && (row_index > 0) && (col_index > 0);
    }
};



class Map{
private:
    std::vector<std::vector<Tile>> Matrix;
    
public:
    Map(){};
    int row(){ return (int)Matrix.size(); }
    int col(){ return (int)Matrix[0].size(); }
    void set_boundary(int row, int col){
        Matrix.resize(row + 2, std::vector<Tile>(col + 2));
        for (int i = 0; i < row + 2; i++){
            Matrix[i][0] = Tile(i, 0, '%');
            Matrix[i][col + 1] = Tile(i, col + 1, '%');
        }
        for (int i = 1; i < col + 2; i++){
            Matrix[0][i] = Tile(0, i, '%');
            Matrix[row + 1][i] = Tile(row + 1, i, '%');
        }
    }
    void print_map(std::ofstream &fout){
        int row = (int)Matrix.size();
        int col = (int)Matrix[0].size();
        for (int i = 0; i < row; i++){
            for (int j = 0; j < col; j++){
                fout << Matrix[i][j].get_color() << " ";
            }
            fout << std::endl;;
        }
    }
    void set_row(std::string &s, int row, int col){
        for (int i = 1; i < col + 1; i++){
            Matrix[row][i] = Tile(row, i, s[i - 1]);
        }
    }
    void set_start_end(int row, int col){
        Matrix[1][1] = Tile(1, 1, '0');
        Matrix[row][col] = Tile(row, col, '0');
    }
    Tile& at(int row, int col){
        return Matrix[row][col];
    }
    
    Tile& above(Tile &cur){
        int row = cur.row();
        int col = cur.col();
        return Matrix[row - 1][col];
    }
    Tile& right(Tile &cur){
        int row = cur.row();
        int col = cur.col();
        return Matrix[row][col + 1];
    }
    Tile& below(Tile &cur){
        int row = cur.row();
        int col = cur.col();
        return Matrix[row + 1][col];
    }
    Tile& left(Tile &cur){
        int row = cur.row();
        int col = cur.col();
        return Matrix[row][col - 1];
    }
    
    // for each tile, check for adjacent tiles in clockwise order start from the the one above.
    // example: above -> right -> below -> left.
    bool BFS_find_path(Tile &start, Tile &destination){
        std::queue<Tile> Q;
        bool arrived = false;
        start.visit();
        Q.push(start);
        while(!Q.empty() && !arrived){
            Tile &cur = Q.front();
            Q.pop();
            if (cur == destination) {
                arrived = true;
                return true;
            }
            if (above(cur).is_valid_to_add_to_queue()){
                Q.push(above(cur));
                above(cur).came_from(cur);
                above(cur).visit();
            }
            if (right(cur).is_valid_to_add_to_queue()){
                Q.push(right(cur));
                right(cur).came_from(cur);
                right(cur).visit();
            }
            if (below(cur).is_valid_to_add_to_queue()){
                Q.push(below(cur));
                below(cur).came_from(cur);
                below(cur).visit();
            }
            if (left(cur).is_valid_to_add_to_queue()){
                Q.push(left(cur));
                left(cur).came_from(cur);
                left(cur).visit();
            }
        }
        return arrived;
    }

    
    void mark_path(Tile &start, Tile &destination){
        Tile *curp = &destination;
        Tile *stp = &start;
        while (curp != stp){
            int row = curp->get_origin().first;
            int col = curp->get_origin().second;
            curp->mark_as_path();
            curp = &Matrix[row][col];
        }
    }
    
};




void set_matrix(Map &Mat, int row, int col, std::ifstream &in);

int main() {
    int row, col;
    
    std::ifstream fin;
    std::ofstream fout;
    
    std::string inFileName;
    std::string outFileName;
    
    std::cout << "Please Enter Input File Name: ";
    std::cin >> inFileName;
    fin.open(inFileName);
    if (fin.fail()){
        std::cout << "In file failed to open." << std::endl;
    }
    std::cout << "Please Enter Output File Name: ";
    std::cin >> outFileName;
    fout.open(outFileName);
    if (fout.fail()){
        std::cout << "Out file failed to open." << std::endl;
    }
    
    std::string line;
    fin >> row >> col;
    
    Map MAP;
    
    MAP.set_boundary(row, col);
    set_matrix(MAP, row, col, fin);
    fout << std::endl;
    
    fout << "Here is the " << row << " X " << col <<" labyrinth; " << std::endl;
    MAP.print_map(fout);
    fout << std::endl;
    
    Tile &start = MAP.at(1,1);
    Tile &dest = MAP.at(row,col);
    
//    std::cout << "Please Select your favorite path finding algorithm; " << std::endl;
//    std::cout << "1 - Breadth First Search" << std::endl;
//    std::cout << "2 - Depth First Search" << std::endl;
//    std::cout << "3 - Dijkstra's algorthm" << std::endl;
//    std::cout << "---> ";
    
    bool found = MAP.BFS_find_path(start, dest);
    
    fout << "Does the path exist? " << std::endl;
    if (found){ fout << "Yes!"; }
    else{ fout << "Nah...";}
    fout << std::endl << std::endl;
    
    if (found) {
        fout << "The path is marked as '*'." << std::endl;
        MAP.mark_path(start, dest);
        MAP.print_map(fout);
        fout << std::endl;
    }
    return 0;
}


void set_matrix(Map &Mat, int row, int col, std::ifstream &in){
    int row_num = 1;
    std::string row_chars;
    while (in >> row_chars && row_num <= row){
        Mat.set_row(row_chars, row_num, col);
        row_num++;
    }
    Mat.set_start_end(row, col);
}













// test for checker functions
//Tile center = test.at(1,1);
//fout << center.row() << " " << center.col() << " " << center.get_color() << std::endl;
//fout << test.above(center).row() << " " << test.above(center).col() <<  " " << test.above(center).get_color() << std::endl;
//fout << test.right(center).row() << " " << test.right(center).col() <<  " " << test.right(center).get_color() << std::endl;
//fout << test.below(center).row() << " " << test.below(center).col() <<  " " << test.below(center).get_color() << std::endl;
//fout << test.left(center).row() << " " << test.left(center).col() <<  " " << test.left(center).get_color() << std::endl;



//fout << "cur index: " << start.row() << ", " << start.col() << std::endl;
//Tile &abv = test.above(start);
//fout << "abv index: " << abv.row() << ", " << abv.col() << std::endl;
//fout << "color: " << abv.get_color() << std::endl;
//fout << "is_blocked: " << abv.is_blocked() << std::endl;
//fout << "is_visited: " << abv.is_visited() << std::endl;
//fout << "is_boundary: " << abv.is_boundary() << std::endl;



//    fout << test.at(2,1).get_origin().first << " ";
//    fout << test.at(2,1).get_origin().second;
//    fout << std::endl << "Should be 1, 1 " << std::endl;
//    fout << test.at(3, 1).get_origin().first << ", ";
//    fout << test.at(3, 1).get_origin().second;
//    fout << std::endl << "Should be 2, 1 ";
