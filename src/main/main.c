#include <stdio.h>
#include <stdlib.h>
#include "../lib/chess.h"

int main(int argc, char* argv[])
{
    ChessBoard board = make_start_board();
    ChessCoordinate_str start_str_coords;
    ChessCoordinate_str end_str_coords;

    // TODO: work on checks
    print_white_board(&board);

    printf("White to move! Enter start and end coordinates, Ctrl+D to end:\n");


    while (scanf("%c%c %c%c", &start_str_coords.file, &start_str_coords.rank, 
                                &end_str_coords.file, &end_str_coords.rank) == 4)
    {
        flush_buffer();

        ChessPiece piece = move_piece(&board, str_to_coordinate(start_str_coords),
                                                str_to_coordinate(end_str_coords));

        if (piece.type != typeNONE)
        {
            printf("Move %c%c from %c%c to %c%c\n", piece_colour_to_char(piece.colour), 
                                                piece_type_to_char(piece.type), 
                                    start_str_coords.file, start_str_coords.rank, 
                                            end_str_coords.file, end_str_coords.rank);
        }

        // handle promotion
        if (piece.type == kPawnPromotion)
        {
            print_board(&board, !board.white_to_move);
            handle_promotion(&board, str_to_coordinate(end_str_coords));
            print_board(&board, board.white_to_move);       
        }
        else
        {
            print_board(&board, board.white_to_move);

            if (piece.type == typeNONE)
            {
                printf("Invalid move! ");
            }
        }
        
        print_who_to_move(&board);

        printf("Enter start and end coordinates, Ctrl+D to end:\n");
    }

    return 0;
}