#include "..\TerminalSynth\Matrix.h"
#include <functional>
#include <iostream>
#include <string>

void Output(const char* message, bool endline)
{
    std::cout << message;

    if (endline)
        std::cout << std::endl;
}
void Output(const std::string& message, bool endline)
{
    Output(message.c_str(), endline);
}
void Output(const Matrix<double>& matrix)
{
    matrix.IterateByValue([&](int row, int column, double value) {
        if (column == 0)
            Output("| ", false);

        Output(std::to_string(value), false);
        Output(" ", false);

        if (column == (matrix.GetDimension(1) - 1))
            Output(" |", true);
    });

    Output("", true);
}
void Test(const char* testName, std::function<bool()> testFunction)
{
    Output(testName, false);

    if (testFunction())
        Output(": Passed", true);
    else
        Output(": Failed", true);
}
int main()
{
    Output("Terminal Synth Unit Tests!", true);
    Output("", true);


    double data1[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
    double data2[3][3] = { {2, 1, 1}, {1, 2, 1}, {1, 1, 2} };

    Matrix<double> matrix1(3, 3);
    Matrix<double> matrix2(3, 3);

    matrix1.SetRow(0, &data1[0][0]);
    matrix1.SetRow(1, &data1[1][0]);
    matrix1.SetRow(2, &data1[2][0]);

    matrix2.SetRow(0, &data2[0][0]);
    matrix2.SetRow(1, &data2[1][0]);
    matrix2.SetRow(2, &data2[2][0]);


    Test("Matrix: IsIdentity", [&]() {
        return matrix1.IsIdentity() == true;
    });

    Test("Matrix: Add", [&]() {        
        matrix1.Add(matrix2);
        return true;
    });

    Test("Matrix: SetFrom", [&]() {
        matrix1.SetFrom(matrix2);
        return true;
    });

    Test("Matrix: Invert", [&]() {
        return matrix2.Invert();
    });

    Test("Matrix: IterateSet", [&]() {

        double preValue = matrix2.Get(0, 0);

        matrix2.IterateSet(0, [&](int row, int column, double* currentRef) {
            return row != 0 || column != 0;
        });

        return matrix2.Get(0,0) == preValue;
    });

    Output(matrix2);

    Output(matrix1);

    Test("Matrix: Mult", [&]() {
        matrix1.Mult(matrix2);
        return true;
    });

    Output(matrix1);
}