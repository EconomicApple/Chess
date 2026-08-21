#include "chess.h"

ChessBoard 
make_empty_board()
{
    ChessBoard board;
    
    for (ChessRank rank = k1; rank <= k8; ++rank)
    {
        for (ChessFile file = kA; file <= kH; ++file)
        {
            board.board[file - kA][rank - k1]= (ChessPiece){
                                colourNONE, typeNONE};
        }
    }

    board.white_to_move = true;
    board.white_king_moved = false;
    board.black_king_moved = false;
    board.a1_rook_moved = false;
    board.a8_rook_moved = false;
    board.h1_rook_moved = false;
    board.h8_rook_moved = false;

    return board;
}


void
init_colour(ChessBoard* p_board, ChessPieceColour colour)
{
    ChessRank rank_num;

    switch (colour)
    {
        case colourNONE:
            return;
        case kWhite:
            rank_num = k1;
            break;
        case kBlack:
            rank_num = k7;
            break;
        default:
            break;
    }

    for (ChessRank rank = rank_num; rank <= rank_num + 1; ++rank)
    {
        for (ChessFile file = kA; file <= kH; ++file)
        {
            fetch_coordinate(p_board, 
                (ChessCoordinate){file, rank})->colour = colour;
        }
    }
}


void
init_pieces(ChessBoard* p_board)
{
    const ChessPieceType piece_positions[BOARD_FILES] = {
                                    kRook, kKnight, kBishop, 
                                    kQueen, kKing, kBishop, 
                                    kKnight, kRook};

    for (ChessFile file = kA; file <= kH; ++file)
    {
        fetch_coordinate(p_board, 
            (ChessCoordinate){file, k1})->type = piece_positions[file - kA];
        fetch_coordinate(p_board, 
            (ChessCoordinate){file, k2})->type = kPawn;
        fetch_coordinate(p_board, 
            (ChessCoordinate){file, k8})->type = piece_positions[file - kA];
        fetch_coordinate(p_board, 
            (ChessCoordinate){file, k7})->type = kPawn;
    }

    p_board->white_king_coordinate = (ChessCoordinate){kE, k1};
    p_board->black_king_coordinate = (ChessCoordinate){kE, k8};
}


ChessBoard
make_start_board()
{
    ChessBoard board = make_empty_board();
    init_colour(&board, kWhite);
    init_colour(&board, kBlack);
    init_pieces(&board);
    return board;
}

ChessPiece*
fetch_coordinate(ChessBoard* p_board, ChessCoordinate coordinates)
{
    if (coordinates.file < kA || coordinates.file > kH 
        || coordinates.rank < k1 || coordinates.rank > k8)
    {   
        return NULL;
    }

    return &(p_board->board[coordinates.file - kA][coordinates.rank - k1]);
}

ChessCoordinate 
shift_ver_coordinate(ChessCoordinate coordinates, int offset)
{
    return (ChessCoordinate){.file = coordinates.file, 
                        .rank = coordinates.rank + offset};
}

ChessCoordinate
shift_hor_coordinate(ChessCoordinate coordinates, int offset)
{
    return (ChessCoordinate){.file = coordinates.file + offset, 
                        .rank = coordinates.rank};
}



void
raw_move_piece(ChessBoard* p_board, 
                ChessCoordinate start_coords,
                    ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    if (p_start_piece != NULL && p_end_piece != NULL)
    {
        *p_end_piece = *p_start_piece;
        *p_start_piece = (ChessPiece){colourNONE, typeNONE};
    }
    
}


ChessPieceType
is_valid_white_pawn_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    if (p_start_piece == NULL || p_end_piece == NULL ||
        p_start_piece->type != kPawn)
    {
        return typeNONE;
    }


    if (end_coords.rank == start_coords.rank + 1)
    {
        bool possible_move = false;

        if (end_coords.file == start_coords.file)
        {
            if (p_end_piece->type != typeNONE)
            {
                return typeNONE;
            }

            possible_move = true;
        }

        if (end_coords.file + 1 == start_coords.file 
            || end_coords.file - 1 == start_coords.file)
        {
            if (p_end_piece->type == typeNONE)
            {
                if (start_coords.rank == k5)
                {
                    ChessCoordinate low_coords = shift_ver_coordinate(
                                                            end_coords, -1);
                    ChessPiece* low_piece = fetch_coordinate(p_board, 
                                        low_coords);

                    if (low_piece->colour == kBlack 
                            && low_piece->type == kPawn 
                            && is_coordinate_equal(
                                    p_board->en_passantable.coordinate, 
                                     low_coords))
                    {
                        return kPawn;
                    }
                }

                return typeNONE;
            }

            possible_move = true;
        }
        
        if (!possible_move)
        {
            return typeNONE;
        }

        if (end_coords.rank == k8)
        {
            return kPawnPromotion;
        }
        
        return kPawn;
    }

    if (start_coords.rank == k2 && end_coords.rank == k4)
    {
        ChessCoordinate lower_rank_coord = end_coords;
        lower_rank_coord.rank = k3;

        if (p_end_piece->type == typeNONE 
            && fetch_coordinate(p_board, lower_rank_coord)->type == typeNONE)
        {
            return kPawn;
        }
    }

    return typeNONE;    
}

ChessPieceType
is_valid_black_pawn_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    if (p_start_piece == NULL || p_end_piece == NULL || 
        p_start_piece->type != kPawn)
    {
        return typeNONE;
    }


    if (end_coords.rank == start_coords.rank - 1)
    {
        bool possible_move = false;

        if (end_coords.file == start_coords.file)
        {
            if (p_end_piece->type != typeNONE)
            {
                return typeNONE;
            }

            possible_move = true;
        }

        if (end_coords.file + 1 == start_coords.file 
            || end_coords.file - 1 == start_coords.file)
        {
            if (p_end_piece->type == typeNONE)
            {
                if (start_coords.rank == k4)
                {
                    ChessCoordinate high_coords = shift_ver_coordinate(
                                                end_coords, 1);
                    ChessPiece* high_piece = fetch_coordinate(p_board, 
                                        high_coords);

                    if (high_piece->colour == kWhite && high_piece->type == kPawn 
                                && is_coordinate_equal(
                                        p_board->en_passantable.coordinate, 
                                        high_coords))
                    {
                        return kPawn;
                    }
                }

                return typeNONE;
            }

            possible_move = true;
        }
        
        if (!possible_move)
        {
            return typeNONE;
        }


        if (end_coords.rank == k1)
        {
            return kPawnPromotion;
        }
        
        return kPawn;
    }

    if (start_coords.rank == k7 && end_coords.rank == k5)
    {
        ChessCoordinate lower_rank_coord = end_coords;
        lower_rank_coord.rank = k6;

        if (p_end_piece->type == typeNONE 
            && fetch_coordinate(p_board, lower_rank_coord)->type == typeNONE)
        {
            return kPawn;
        }
    }

    return typeNONE;    
}


bool
is_coordinate_equal(ChessCoordinate c1, ChessCoordinate c2)
{
    return c1.file == c2.file && c1.rank == c2.rank;
}

ChessPieceType
is_valid_pawn_move(ChessBoard* p_board, 
                    ChessCoordinate start_coords, 
                    ChessCoordinate end_coords)
{
    // Guaranteed to be within bounds due to fetch coordinates
    
    if (fetch_coordinate(p_board, start_coords)->colour == kWhite)
    {
        if (p_board->white_to_move)
        {
            return is_valid_white_pawn_move(p_board, start_coords, end_coords);
        }
        
        return typeNONE;
    }
    else
    {
        if (!p_board->white_to_move)
        {
            return is_valid_black_pawn_move(p_board, start_coords, end_coords);
        }
        
        return typeNONE;
    }
}

ChessPieceType
is_valid_knight_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    // Catch conds early
    if (p_start_piece == NULL || p_end_piece == NULL || p_start_piece->type != kKnight
        || p_end_piece->colour == p_start_piece->colour)
    {
        return typeNONE;
    }

    // Movement
    return ((abs(end_coords.file - start_coords.file) == 2 
                && abs(end_coords.rank - start_coords.rank) == 1)
            || (abs(end_coords.file - start_coords.file) == 1 
                && abs(end_coords.rank - start_coords.rank) == 2));
                
}


ChessPieceType
is_valid_bishop_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    // Catch conds early
    if (p_start_piece == NULL || p_end_piece == NULL || p_start_piece->type != kBishop
        || abs(end_coords.file - start_coords.file) 
            != abs(end_coords.rank - start_coords.rank)
            || p_start_piece == p_end_piece
            || p_end_piece->colour == p_start_piece->colour)
    {
        return typeNONE;
    }   


    int ver_increment = 2 * (end_coords.rank > start_coords.rank) - 1;
    int hor_increment = 2 * (end_coords.file > start_coords.file) - 1;

    start_coords.rank += ver_increment;
    start_coords.file += hor_increment;

    // Bounded by fetch coordinate
    while (start_coords.file != end_coords.file)
    {
        if (fetch_coordinate(p_board, start_coords)->type != typeNONE)
        {
            return typeNONE;
        }

        start_coords.rank += ver_increment;
        start_coords.file += hor_increment;
    }

    return kBishop;
}

ChessPieceType
is_valid_rook_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    // Catch conds early
    if (p_start_piece == NULL || p_end_piece == NULL || p_start_piece->type != kRook
        || (end_coords.file != start_coords.file 
            && end_coords.rank != start_coords.rank)
        || p_start_piece == p_end_piece
        || p_end_piece->colour == p_start_piece->colour)
    {
        return typeNONE;
    }   

    if (end_coords.rank != start_coords.rank)
    {
        int ver_increment = 2 * (end_coords.rank > start_coords.rank) - 1;
        start_coords.rank += ver_increment;

        while (start_coords.rank != end_coords.rank)
        {
            if (fetch_coordinate(p_board, start_coords)->type != typeNONE)
            {
                return typeNONE;
            }

            start_coords.rank += ver_increment;
        }
    }
    else
    {
        int hor_increment = 2 * (end_coords.file > start_coords.file) - 1;

        start_coords.file += hor_increment;

        while (start_coords.file != end_coords.file)
        {
            if (fetch_coordinate(p_board, start_coords)->type != typeNONE)
            {
                return typeNONE;
            }

            start_coords.file += hor_increment;
        }
    }
    return kRook;
}

ChessPieceType
is_valid_queen_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

   if (p_start_piece == NULL || p_end_piece == NULL 
        || p_start_piece->type != kQueen
        || p_start_piece == p_end_piece
        || p_end_piece->colour == p_start_piece->colour)
    {
        return typeNONE;
    }

    int ver_increment = 2 * (end_coords.rank > start_coords.rank) - 1;
    int hor_increment = 2 * (end_coords.file > start_coords.file) - 1;

    // Bishop specific constraints
    if (abs(end_coords.file - start_coords.file) 
            == abs(end_coords.rank - start_coords.rank))
    {
        ChessCoordinate temp_start = start_coords;

        temp_start.rank += ver_increment;
        temp_start.file += hor_increment;

        // Bounded by fetch coordinate
        while (temp_start.file != end_coords.file)
        {
            if (fetch_coordinate(p_board, temp_start)->type != typeNONE)
            {
                break;
            }

            temp_start.rank += ver_increment;
            temp_start.file += hor_increment;
        }

        // Bishop success
        if (temp_start.file == end_coords.file)
        {
            return kQueen;
        }
    }

    // Rook specific constraints
    if (end_coords.file != start_coords.file 
        && end_coords.rank != start_coords.rank)
    {
        return typeNONE;
    }

    if (end_coords.rank != start_coords.rank)
    {
        start_coords.rank += ver_increment;

        while (start_coords.rank != end_coords.rank)
        {
            if (fetch_coordinate(p_board, start_coords)->type != typeNONE)
            {
                return typeNONE;
            }

            start_coords.rank += ver_increment;
        }
    }
    else
    {
        start_coords.file += hor_increment;

        while (start_coords.file != end_coords.file)
        {
            if (fetch_coordinate(p_board, start_coords)->type != typeNONE)
            {
                return typeNONE;
            }

            start_coords.file += hor_increment;
        }
    }

    return kQueen;
}


ChessPieceType
is_valid_king_move(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    ChessPiece* p_start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* p_end_piece = fetch_coordinate(p_board, end_coords);

    if (p_start_piece == NULL || p_end_piece == NULL 
        || p_start_piece->type != kKing
        || p_start_piece == p_end_piece
        || p_end_piece->colour == p_start_piece->colour)
    {
        return typeNONE;
    }

    if (abs(end_coords.file - start_coords.file) <= 1
        && abs(end_coords.rank - start_coords.rank) <= 1)
    {
        return kKing;
    }

    return typeNONE;
}


bool
is_valid_castle(ChessBoard* p_board, ChessCoordinate end_coords)
{
    ChessCoordinate king_coordinate;

    if (p_board->white_to_move)
    {
        if (p_board->white_king_moved
            || end_coords.rank != k1){return false;}
        king_coordinate = p_board->white_king_coordinate;
    }
    else
    {
        if (p_board->black_king_moved
            || end_coords.rank != k8){return false;}
        king_coordinate = p_board->black_king_coordinate;
    }


    bool going_right;

    // Check if rook has moved
    if (end_coords.file == kG)
    {
        if (p_board->white_to_move)
        {
            if (p_board->h1_rook_moved){return false;}
        }
        else
        {
            if (p_board->h8_rook_moved){return false;}
        }

        going_right = true;
    }
    else if (end_coords.file == kC)
    {
        if (p_board->white_to_move)
        {
            if (p_board->a1_rook_moved){return false;}
        }
        else
        {
            if (p_board->a8_rook_moved){return false;}
        }

        going_right = false;
    }
    else
    {
        return false;
    }

    int increment = going_right * 2 - 1;

    ChessCoordinate check_coordinate = king_coordinate;
    check_coordinate.file += increment;

    // Check if all the cells between are empty
    for (;check_coordinate.file < kH && check_coordinate.file > kA; 
        check_coordinate.file += increment)
    {
        // No need to check bounds since king coordinate is always within bounds.
        if (fetch_coordinate(p_board, check_coordinate)->type != typeNONE)
        {
            return false;
        }
    }

    // Check for illegal checks
    // Required due to the check
    p_board->white_to_move = !p_board->white_to_move;
    ChessCoordinate dest_coordinate = check_coordinate;

    for (check_coordinate = king_coordinate;
            check_coordinate.file < kG && check_coordinate.file > kC; 
            check_coordinate.file += increment)
    {
        if (in_illegal_check(p_board))
        {
            // Move king back to start
            raw_move_piece(p_board, dest_coordinate, king_coordinate);
            p_board->white_to_move = !p_board->white_to_move;
            return false;
        }

        // Move king one step towards increment direction.
        dest_coordinate = check_coordinate;
        dest_coordinate.file += increment;
        raw_move_piece(p_board, check_coordinate, dest_coordinate);
        
        // Update king coordinate
        if (is_coordinate_equal(king_coordinate, (ChessCoordinate){kE, k1}))
        {
            p_board->white_king_coordinate = dest_coordinate;
        }
        else
        {
            p_board->black_king_coordinate = dest_coordinate;
        }
    }

    // Move king back to start
    raw_move_piece(p_board, end_coords, king_coordinate);
    p_board->white_to_move = !p_board->white_to_move;
    return true;
}


bool
hor_ver_check(ChessBoard* p_board)
{
    ChessCoordinate king_coordinate;

    if (p_board->white_to_move)
    {
        king_coordinate = p_board->black_king_coordinate;
    }
    else
    {
        king_coordinate = p_board->white_king_coordinate;
    }

    ChessPiece* p_king_piece = fetch_coordinate(p_board, king_coordinate);

    ChessPiece* p_checking_piece;

    ChessFile file = king_coordinate.file;
    ChessRank rank;

    // Go vertically

    for (rank = king_coordinate.rank + 1; rank <= k8; ++rank)
    {
        p_checking_piece = fetch_coordinate(p_board, (ChessCoordinate){file, rank});

        if (p_checking_piece->colour != p_king_piece->colour
            && (p_checking_piece->type == kRook || p_checking_piece->type == kQueen))
        {
            return true;
        }
        else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
        {
            break;
        }
    }

    for (rank = king_coordinate.rank - 1; rank >= k1; --rank)
    {
        p_checking_piece = fetch_coordinate(p_board, (ChessCoordinate){file, rank});

        if (p_checking_piece->colour != p_king_piece->colour
            && (p_checking_piece->type == kRook || p_checking_piece->type == kQueen))
        {
            return true;
        }
        else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
        {
            break;
        }
    }

    rank = king_coordinate.rank;

    // Go horizontally

    for (file = king_coordinate.file + 1; file <= kH; ++file)
    {
        p_checking_piece = fetch_coordinate(p_board, (ChessCoordinate){file, rank});

        if (p_checking_piece->colour != p_king_piece->colour
            && (p_checking_piece->type == kRook || p_checking_piece->type == kQueen))
        {
            return true;
        }
        else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
        {
            break;
        }
    }

    for (file = king_coordinate.file - 1; file >= kA; --file)
    {
        p_checking_piece = fetch_coordinate(p_board, (ChessCoordinate){file, rank});

        if (p_checking_piece->colour != p_king_piece->colour
            && (p_checking_piece->type == kRook || p_checking_piece->type == kQueen))
        {
            return true;
        }
        else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
        {
            break;
        }
    }

    return false;
}


bool
diag_check(ChessBoard* p_board)
{
    ChessCoordinate king_coordinate;

    if (p_board->white_to_move)
    {
        king_coordinate = p_board->black_king_coordinate;
    }
    else
    {
        king_coordinate = p_board->white_king_coordinate;
    }

    ChessPiece* p_king_piece = fetch_coordinate(p_board, king_coordinate);

    if (p_king_piece == NULL)
    {
        printf("King out of bounds!!!!!!\n");
        exit(EXIT_FAILURE);
    }

    
    // Go top left and top right
    for (int i = -1; i < 2; i = i + 2)
    {
        ChessCoordinate check_coords = king_coordinate;
        check_coords.rank++;
        check_coords.file += i;

        ChessPiece* p_checking_piece = fetch_coordinate(p_board, check_coords);

        // Check if a pawn is checking the king
        if (p_checking_piece != NULL 
            && p_checking_piece->colour == kBlack
            && p_checking_piece->type == kPawn 
            && p_king_piece->type == kWhite)
        {
            return true;
        }

        while (p_checking_piece != NULL)
        {
            if (p_checking_piece->colour != p_king_piece->colour 
                && (p_checking_piece->type == kBishop || p_checking_piece->type == kQueen))
            {
                return true;
            }
            else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
            {
                break;
            }

            check_coords.rank++;
            check_coords.file += i;

            p_checking_piece = fetch_coordinate(p_board, check_coords);
        }
    }


    // Go bottom left and bottom right
    for (int i = -1; i < 2; i = i + 2)
    {
        ChessCoordinate check_coords = king_coordinate;
        check_coords.rank--;
        check_coords.file += i;

        ChessPiece* p_checking_piece = fetch_coordinate(p_board, check_coords);

        // Check if a pawn is checking the king
        if (p_checking_piece != NULL 
            && p_checking_piece->colour == kWhite
            && p_checking_piece->type == kPawn 
            && p_king_piece->type == kBlack)
        {
            return true;
        }

        while (p_checking_piece != NULL)
        {
            if (p_checking_piece->colour != p_king_piece->colour 
                && (p_checking_piece->type == kBishop || p_checking_piece->type == kQueen))
            {
                return true;
            }
            else if (!(p_checking_piece->type == typeNONE)) // This will block the rest
            {
                break;
            }

            check_coords.rank++;
            check_coords.file += i;

            p_checking_piece = fetch_coordinate(p_board, check_coords);
        }
    }

    return false;
}

bool
knight_check(ChessBoard* p_board)
{
    ChessCoordinate king_coordinate;

    if (p_board->white_to_move)
    {
        king_coordinate = p_board->black_king_coordinate;
    }
    else
    {
        king_coordinate = p_board->white_king_coordinate;
    }

    ChessPiece* p_king_piece = fetch_coordinate(p_board, king_coordinate);

    if (p_king_piece == NULL)
    {
        printf("King out of bounds!!!!!!\n");
        exit(EXIT_FAILURE);
    }

    ChessCoordinate check_coords = (ChessCoordinate){king_coordinate.file + 2,       
                                        king_coordinate.rank + 1};

                            
    // Check all cells for a knight
    int rank_direction = -2;
    int file_direction = 0;

    for (int i = 0; i < 8; ++i)
    {
        ChessPiece* p_checking_piece = fetch_coordinate(p_board, check_coords);

        if (p_checking_piece != NULL 
            && p_checking_piece->colour != p_king_piece->colour 
            && p_checking_piece->type == kKnight)
        {
            return true;
        }

        // update check_coords
        check_coords.rank += rank_direction;
        check_coords.file += file_direction;

        rank_direction += 2 * (i <= 3) - 1;
        file_direction += 2 * (i > 1) - 1;
    }

    return false;
}

bool 
in_illegal_check(ChessBoard* p_board)
{
    return (hor_ver_check(p_board) || diag_check(p_board) || knight_check(p_board));
}


ChessFile
char_to_file(char c)
{
    c = tolower(c);

    if ('a' <= c && c <= 'h')
    {
        return (kA + (c - 'a'));
    }

    return fileNONE;
}


ChessRank
char_to_rank(char c)
{
    if ('1' <= c && c <= '8')
    {
        return (k1 + (c - '1'));
    }

    return rankNONE;
}


char
file_to_char(ChessFile file)
{
    return ((file - kA) + 'a');
}

char
rank_to_char(ChessRank rank)
{
    return ((rank - k1)+ '1');
}


void 
print_white_board(ChessBoard* p_board)
{
    setlocale(LC_CTYPE, "");

    for (ChessRank rank = k8; rank > rankNONE; --rank)
    {
        printf("%c ", rank - k1 + '1');

        for (ChessFile file = kA; file <= kH; ++file)
        {
            printf("%lc ", piece_to_unicode(fetch_coordinate(p_board, 
                                (ChessCoordinate){
                                        .file = file, .rank = rank})));
        }
        putchar('\n');
    }  

    printf("  ");

    for (ChessFile file = kA; file <= kH; ++file)
    {
        printf("%c ", file - kA + 'a');
    }

    putchar('\n');
}


void 
print_black_board(ChessBoard* p_board)
{
    setlocale(LC_CTYPE, "");

    for (ChessRank rank = k1; rank <= k8; ++rank)
    {
        printf("%c ", rank - k1 + '1');

        for (ChessFile file = kA; file <= kH; ++file)
        {
            
            printf("%lc ", piece_to_unicode(fetch_coordinate(p_board, 
                            (ChessCoordinate){.file = file, .rank = rank})));
        }
        putchar('\n');
    }  

    printf("  ");

    for (ChessFile file = kA; file <= kH; ++file)
    {
        printf("%c ", file - kA + 'a');
    }

    putchar('\n');
}


void 
print_board(ChessBoard* p_board, bool white_perspective)
{
    if (white_perspective)
    {
        print_white_board(p_board);
    }
    else
    {
        print_black_board(p_board);
    }
}

void
print_who_to_move(ChessBoard* p_board)
{
    if (p_board->white_to_move)
    {
        printf("White to move! ");
    }
    else
    {
        printf("Black to move! ");
    }
}
wchar_t
piece_to_unicode(ChessPiece* p_piece)
{
    if (p_piece->type == kPawnPromotion)
    {
        return '?';
    }

    if (p_piece->type == typeNONE)
    {
        return SQUARE;
    }

    int hash = (kKing - typeNONE) 
            * (p_piece->colour == kWhite) + kKing - p_piece->type;

    return WHITE_KING + hash;
}

char
piece_type_to_char(ChessPieceType piece_type)
{
    switch (piece_type)
    {
        case typeNONE: return ' ';
        case kPawn: return 'P';
        case kKnight: return 'N';
        case kBishop: return 'B';
        case kRook: return 'R';
        case kQueen: return 'Q';
        case kKing: return 'K';
        case kPawnPromotion: return '?';
        default:
            printf("Invalid piece type\n");
            exit(EXIT_FAILURE);
    }
}

ChessPieceType
char_to_promotion_piece_type(char c)
{
    switch (c)
    {
        case 'N': return kKnight;
        case 'B': return kBishop;
        case 'R': return kRook;
        case 'Q': return kQueen;
        default: return typeNONE;
    }
}

void
handle_promotion(ChessBoard* p_board, 
        ChessCoordinate promotion_coordinates)
{
    printf("Promoted! Enter which piece to promote to."
                    "\nN for knight, B for bishop, R for rook, Q for queen: ");

    char c;
            
    ChessPieceType piece_type;

    while ((c = getchar()) != EOF
            && (piece_type = char_to_promotion_piece_type(c)) == typeNONE){}

    flush_buffer();

    ChessPiece* p_piece = fetch_coordinate(p_board, promotion_coordinates);

    if (p_piece != NULL)
    {
        p_piece->type = piece_type;
    }
}


ChessCoordinate 
str_to_coordinate(ChessCoordinate_str str_coordinate)
{
    return (ChessCoordinate){.file = char_to_file(str_coordinate.file), 
                        .rank = char_to_rank(str_coordinate.rank)};
}

ChessCoordinate_str
coordinates_to_coord_str(ChessCoordinate coordinate)
{
    return (ChessCoordinate_str){.file = file_to_char(coordinate.file),
                            .rank = rank_to_char(coordinate.rank)};
}

void flush_buffer()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF){};
}

char 
piece_colour_to_char(ChessPieceColour piece_colour)
{
    if (piece_colour == kWhite)
    {
        return 'W';
    }
    else if (piece_colour == kBlack)
    {
        return 'B';
    }
    
    return ' ';
}


ChessPiece
move_piece(ChessBoard* p_board, 
                        ChessCoordinate start_coords, 
                        ChessCoordinate end_coords)
{
    // If it passes back to the same person, en passant is no longer possible
    if (p_board->en_passantable.on_turn == p_board->white_to_move)
    {
        p_board->en_passantable.coordinate = (ChessCoordinate){
                                        fileNONE, rankNONE};
    }

    ChessPiece* start_piece = fetch_coordinate(p_board, start_coords);
    ChessPiece* end_piece = fetch_coordinate(p_board, end_coords);

    if (start_piece == NULL || end_piece == NULL)
    {
        return (ChessPiece){colourNONE, typeNONE};
    }

    switch (start_piece->type)
    {
        case kPawn:
            ChessPieceType valid_pawn_move = is_valid_pawn_move(p_board, 
                            start_coords, 
                            end_coords);

            if (valid_pawn_move != typeNONE)
            {
                // if en passant
                if (end_piece->colour == colourNONE &&
                    end_piece->type == typeNONE &&
                    (end_coords.file + 1 == start_coords.file 
                    || end_coords.file - 1 == start_coords.file))
                {
                    int vertical_offset = 1 - 2 * (
                                    start_piece->colour == kWhite);

                    
                    *fetch_coordinate(p_board, 
                        shift_ver_coordinate(end_coords, vertical_offset)) = (
                                (ChessPiece){colourNONE, typeNONE});
                }

                // If a pawn is moved two spaces, set to en passantable.
                if (end_coords.rank == start_coords.rank + 2 
                    || end_coords.rank == start_coords.rank - 2)
                {
                    p_board->en_passantable = (EnPassantable){
                                            .coordinate = end_coords, 
                                            .on_turn = p_board->white_to_move};
                }

                start_piece->type = valid_pawn_move;
                break;
            }

            return (ChessPiece){colourNONE, typeNONE};

        case kKnight:
            if (is_valid_knight_move(p_board, start_coords, end_coords) != typeNONE)
            {
                break;
            }

            return (ChessPiece){colourNONE, typeNONE};
        case kBishop:
            if (is_valid_bishop_move(p_board, start_coords, end_coords) != typeNONE)
            {
                break;
            }

            return (ChessPiece){colourNONE, typeNONE};
        case kRook:
            if (is_valid_rook_move(p_board, start_coords, end_coords) != typeNONE)
            {
                break;
            }

            return (ChessPiece){colourNONE, typeNONE};
        case kQueen:
            if (is_valid_queen_move(p_board, start_coords, end_coords) != typeNONE)
            {
                break;
            }

            return (ChessPiece){colourNONE, typeNONE};
        case kKing:
            if (is_valid_castle(p_board, end_coords))
            {
                // Move the rook
                if (end_coords.file == kC)
                {
                    raw_move_piece(p_board, 
                            (ChessCoordinate){kA, start_coords.rank}, 
                            (ChessCoordinate){kD, start_coords.rank});

                    if (start_coords.rank == k1)
                    {
                        p_board->a1_rook_moved = true;
                    }
                    else
                    {
                        p_board->a8_rook_moved = true;
                    }
                }
                else
                {
                    raw_move_piece(p_board, 
                            (ChessCoordinate){kH, start_coords.rank}, 
                            (ChessCoordinate){kF, start_coords.rank});
                    
                    if (start_coords.rank == k1)
                    {
                        p_board->h1_rook_moved = true;
                    }
                    else
                    {
                        p_board->h8_rook_moved = true;
                    }
                }

                p_board->white_to_move = !p_board->white_to_move;
                raw_move_piece(p_board, start_coords, end_coords);
                return *end_piece;
            }
            else if (is_valid_king_move(p_board, start_coords, end_coords) != typeNONE)
            {
                if (p_board->white_to_move)
                {
                    p_board->white_king_coordinate = end_coords;
                    p_board->white_king_moved = true;
                }
                else
                {
                    p_board->black_king_coordinate = end_coords;
                    p_board->black_king_moved = true;
                }

                break;
            }

            return (ChessPiece){colourNONE, typeNONE};
        default:
            return (ChessPiece){colourNONE, typeNONE};
    }

    ChessPiece temp_piece = *end_piece;

     // Simulate a move then see if is in check
    raw_move_piece(p_board, start_coords, end_coords);
    p_board->white_to_move = !p_board->white_to_move;

    // Reverse the move if it is illegal
    if (in_illegal_check(p_board))
    {
        p_board->white_to_move = !p_board->white_to_move;
        raw_move_piece(p_board, end_coords, start_coords);
        *end_piece = temp_piece;
        return (ChessPiece){colourNONE, typeNONE};
    }

    if (start_piece->type == kRook)
    {
        if (is_coordinate_equal(start_coords, (ChessCoordinate){kA, k1}))
        {
            p_board->a1_rook_moved = true;
        }
        else if (is_coordinate_equal(start_coords, (ChessCoordinate){kA, k8}))
        {
            p_board->a8_rook_moved = true;
        }
        else if (is_coordinate_equal(start_coords, (ChessCoordinate){kH, k1}))
        {
            p_board->h1_rook_moved = true;
        }
        else if (is_coordinate_equal(start_coords, (ChessCoordinate){kH, k8}))
        {
            p_board->h8_rook_moved = true;
        }
    }
   
    return *end_piece;
}