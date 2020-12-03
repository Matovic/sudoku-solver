#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

class Sudoku 
{
public:
	Sudoku(size_t size = 9) : m_vector_box{ size, std::vector<unsigned short>(size, 0) } {}
	~Sudoku(){}

	void m_setOneBox(const unsigned short&, const size_t&, const size_t&);
	unsigned short m_getOneBox(const size_t&, const size_t&);

	int check_number_in_sudoku(const unsigned short&, const size_t&, const size_t&);
	int check_sudoku();
	int free_box_coordinates(size_t* row_free, size_t* column_free);
	int solve();
	int read_line(const std::string& str_line);

private:
	std::vector<std::vector<unsigned short>> m_vector_box;

	friend std::ostream& operator<<(std::ostream&, const Sudoku&);
};

std::ostream& operator<<(std::ostream& stream, const Sudoku& sudoku)
{
	for (size_t row = 0; row < 9; ++row)
		for (size_t column = 0; column < 9; ++column)
			stream << sudoku.m_vector_box[row][column];
	stream << '\n';

	return stream;
}

void Sudoku::m_setOneBox(const unsigned short& ui_box, const size_t& row, const size_t& column)
{
	this->m_vector_box[row][column] = ui_box;
}

unsigned short Sudoku::m_getOneBox(const size_t& row, const size_t& column)
{
	return this->m_vector_box[row][column];
}

int Sudoku::check_number_in_sudoku(const unsigned short& ui_number, const size_t& row, const size_t& column)
{
	//chceck in row and column
	for (size_t index = 0; index < 9; ++index)
	{
		if (index != column && ui_number == this->m_getOneBox(row, index)) 
			return 1;
		if (index != row && ui_number == this->m_getOneBox(index, column)) 
			return 1;
	}

	//check in square
	size_t square_row = row - (row % 3), square_column = column - (column % 3);
	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 3; ++j)
			if (j + square_column != column && i + square_row != row &&
				ui_number == this->m_getOneBox(i + square_row, j + square_column)) 
				return 1;
	return 0;
}

int Sudoku::check_sudoku()
{
	for (size_t i = 0; i < 9; ++i)
		for (size_t j = 0; j < 9; ++j)
			if (this->check_number_in_sudoku(this->m_getOneBox(i, j), i, j))
				return 1;
	return 0;
}

int Sudoku::free_box_coordinates(size_t* row_free, size_t* column_free)
{
	for (size_t row = 0; row < 9; ++row)
		for (size_t column = 0; column < 9; ++column)
		{
			if (this->m_getOneBox(row, column) != 0)
				continue;

			*row_free = row;																		//free coordinate of an row
			*column_free = column;																	//free coordinate of a column

			return 0;
		}
	return 1;
}

int Sudoku::solve()
{
	size_t free_row = 10, free_column = 10;
	if (this->free_box_coordinates(&free_row, &free_column)) if (this->check_sudoku()) return 1;	//find free box row and column
	if (free_row == 10 || free_column == 10) return 0;												//stop recursion

	//try numbers 1-9
	for (unsigned short test_number = 1; test_number < 10; ++test_number) 
		if (!this->check_number_in_sudoku(test_number, free_row, free_column))						//check if test_number is right in sudoku
		{
			this->m_setOneBox(test_number, free_row, free_column);									//add test_number to sudoku
			if (!this->solve()) return 0;															//recursion

			this->m_setOneBox(0, free_row, free_column);											//test_number was wrong - need to change it back to 0
		}
	return 1;
}

int Sudoku::read_line(const std::string& str_line)
{
	for (size_t row = 0; row < 9; ++row)
		for (size_t column = 0; column < 9; ++column)
		{
			//check for bad input
			if ((str_line[9 * row + column] < '0' || str_line[9 * row + column] > '9') && str_line[9 * row + column] != '.')
				return 1;																		

			unsigned short us_number = str_line[9 * row + column] - '0';
			if (str_line[9 * row + column] >= '1' && str_line[9 * row + column] <= '9')
				this->m_setOneBox(us_number, row, column);
		}
	return 0;
}

int read_input(std::istream* input_stream, std::ostream* output_stream)
{
	std::string str_line = "";
	for (unsigned int ui_file_row = 1; std::getline(*input_stream, str_line); ++ui_file_row)
	{
		size_t size_line = str_line.size();
		if (size_line != 81)
		{
			std::cerr << "ERROR 02: Line with invalid length at " << ui_file_row << "\n";
			return 1;
		}

		Sudoku sudoku{};
		if (sudoku.read_line(str_line)) 
		{
			std::cerr << "ERROR 02: Line with invalid input(" << ui_file_row << ")\n";
			return 1;
		}
		if(!sudoku.solve()) *output_stream << sudoku;

		else *output_stream << '\n';
	}
	return 0;
}

int read_file_input(std::istream** input_stream, std::ifstream* file_input, int argc, char* argv[])
{
	if (!strcmp(argv[1], "-i"))													
	{
		file_input->open(argv[2]);
		*input_stream = file_input;
		return 0;
	}
	
	if (argc < 4 && !strcmp(argv[1], "-o"))									//if argv[1] is "-o" AND no input switch and file was given - return
		return 1;

	if (!strcmp(argv[3], "-i"))
	{
		file_input->open(argv[4]);
		*input_stream = file_input;
		return 0;
	}

	return 1;
}

void read_file_output(std::ostream** output_stream, std::ofstream* file_output, char* argv[])
{
	if (!strcmp(argv[1], "-o"))
		file_output->open(argv[2]);

	else if (!strcmp(argv[3], "-o"))
		file_output->open(argv[4]);

	*output_stream = file_output;
}

int main(int argc, char* argv[])
{
	if (argc == 2 || argc == 4 || argc > 5)
	{
		std::cerr << "ERROR 00: Invalid parameters\n";
		return 1;
	}

	std::istream* input_stream = &std::cin;										//if no input file  - read from standart input
	std::ostream* output_stream = &std::cout;									//if no output file - print on standart output

	std::ifstream file_input;
	std::ofstream file_output;

	//arguments need to be at least 3 - path, switch(-i or -o) and name of file
	//read input file
	if (argc > 2)
		if (!read_file_input(&input_stream, &file_input, argc, argv))
			if (!file_input.is_open())
			{
				std::cerr << "ERROR 01: File cannot be open\n";
				return 1;
			}

	//read output file
	if (argc > 4 || (argc > 2 && !file_input.is_open()))
	{
		read_file_output(&output_stream, &file_output, argv);
		if (!file_output.is_open())
		{
			std::cerr << "ERROR 01: File cannot be open\n";
			return 1;
		}
	}

	if (read_input(input_stream, output_stream)) return 1;

	return 0;
}
