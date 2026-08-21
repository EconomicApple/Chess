#ifndef FOA_C_SRC_LIB_CHESS_H
#define FOA_C_SRC_LIB_CHESS_H


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#define BOARD_FILES 8
#define BOARD_RANKS 8
#define WHITE_KING 0x2654
#define SQUARE 0x2581

typedef enum ChessFile
{
    fileNONE,
    kA,
    kB,
    kC,
    kD,
    kE,
    kF,
    kG,
    kH
} ChessFile;


typedef enum ChessRank
{
    rankNONE,
    k1,
    k2,
    k3,
    k4,
    k5,
    k6,
    k7,
    k8
} ChessRank;

typedef enum ChessPieceType
{
    typeNONE,
    kPawn,
    kKnight,
    kBishop,
    kRook,
    kQueen,
    kKing,
    kPawnPromotion
} ChessPieceType;


typedef enum ChessPieceColour
{
    colourNONE,
    kWhite,
    kBlack
} ChessPieceColour;


typedef struct ChessPiece
{
    ChessPieceColour colour;
    ChessPieceType type;
} ChessPiece;

typedef struct ChessCoordinate
{
    ChessFile file;
    ChessRank rank;
} ChessCoordinate;

typedef struct ChessCoordinate_str
{
    char file;
    char rank;
} ChessCoordinate_str;


// Struct used to check if something can be en passanted
typedef struct 
EnPassantable
{
    ChessCoordinate coordinate;
    // Whose turn it was when they played the move
    bool on_turn;
} EnPassantable;


typedef struct ChessBoard
{
    ChessPiece board[BOARD_FILES][BOARD_RANKS];
    EnPassantable en_passantable;
    ChessCoordinate white_king_coordinate;
    bool white_king_moved;
    ChessCoordinate black_king_coordinate;
    bool black_king_moved;

    bool a1_rook_moved;
    bool h1_rook_moved;
    bool h8_rook_moved;
    bool a8_rook_moved;
    
    bool white_to_move;
} ChessBoard;




// Functions for initialising board
ChessBoard make_empty_board();
void init_colour(ChessBoard* p_board, ChessPieceColour colour);
void init_pieces(ChessBoard* p_board);
ChessBoard make_start_board();


// Functions to handle coordinates


// Returns piece address with bounds checking. Returns NULL if out of bounds
ChessPiece* fetch_coordinate(ChessBoard* p_board, ChessCoordinate coordinates);

// Returns coordinate at offset in vertical up direction.
ChessCoordinate shift_ver_coordinate(ChessCoordinate coordinates, int offset);

// Returns coordinate at offset in horizontal right direction.
ChessCoordinate shift_hor_coordinate(ChessCoordinate coordinates, int offset);

bool is_coordinate_equal(ChessCoordinate c1, ChessCoordinate c2);

// Move validations

// Returns typeNONE if invalid, kPawn if valid but cannot promote, kPawnPromotion if can promote
ChessPieceType is_valid_white_pawn_move(ChessBoard* p_board, 
                                        ChessCoordinate start_coords, 
                                        ChessCoordinate end_coords);

// Returns typeNONE if invalid, kPawn if valid but cannot promote, kPawnPromotion if can promote
ChessPieceType is_valid_black_pawn_move(ChessBoard* p_board, 
                                        ChessCoordinate start_coords, 
                                        ChessCoordinate end_coords);

// Returns typeNONE if invalid, kPawn if valid but cannot promote, kPawnPromotion if can promote
ChessPieceType is_valid_pawn_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

// Returns typeNONE if invalid, kKnight if valid
ChessPieceType is_valid_knight_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

// Returns typeNONE if invalid, kBishop if valid
ChessPieceType is_valid_bishop_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

// Returns typeNONE if invalid, kRook if valid
ChessPieceType is_valid_rook_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

// Returns typeNONE if invalid, kQueen if valid
ChessPieceType is_valid_queen_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

// Returns typeNONE if invalid, kKing if valid
ChessPieceType is_valid_king_move(ChessBoard* p_board, 
                                    ChessCoordinate start_coords, 
                                    ChessCoordinate end_coords);

bool is_valid_castle(ChessBoard* p_board,
                                ChessCoordinate end_coords);

// Returns the winner. The king checked is the player whose turn it is to move.
ChessPieceColour is_checkmate(ChessBoard* p_board);

// Returns true if player is in check and is opponent's turn.
bool in_illegal_check(ChessBoard* p_board);

// Checks for horizontal and vertical checks
bool hor_ver_check(ChessBoard* p_board);

// Checks for diagonal checks
bool diag_check(ChessBoard* p_board);

// Checks for knight checks
bool knight_check(ChessBoard* p_board);

// Moves piece from start_coords to end_coords. 
// To facilitate promotion, change what is at the start coords first.
void raw_move_piece(ChessBoard* p_board, 
                    ChessCoordinate start_coords, 
                    ChessCoordinate end_coords);

// Returns NONE piece if invalid move, value of piece moved otherwise.
// Returns kPawnPromotion if eligible to promote.
ChessPiece move_piece(ChessBoard* p_board, 
                    ChessCoordinate start_coords, 
                    ChessCoordinate end_coords);

// Get promotion piece from stdin
void handle_promotion(ChessBoard* p_board, 
                    ChessCoordinate promotion_coordinates);

// Print functions
void print_white_board(ChessBoard* p_board);
void print_black_board(ChessBoard* p_board);

// Print board from perspective
void print_board(ChessBoard* p_board, bool white_perspective);
void print_who_to_move(ChessBoard* p_board);

// Flushes stdin buffer
void flush_buffer();

// Type conversions
ChessFile char_to_file(char c);
ChessRank char_to_rank(char c);
ChessCoordinate str_to_coordinate(ChessCoordinate_str str_coordinate);
ChessCoordinate_str coordinates_to_coord_str(ChessCoordinate coordinates);
char piece_type_to_char(ChessPieceType piece_type);
char piece_colour_to_char(ChessPieceColour piece_colour);
wchar_t piece_to_unicode(ChessPiece* p_piece);
ChessPieceType char_to_promotion_piece_type(char c);


#endif