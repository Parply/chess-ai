#pragma once
#include <vector>
#include <string>
#include <deque>

#include <string.h>

using namespace std;

class chess
{
	public:
		static int getPieceColour(char chPiece);
		static bool isWhitePiece(char chPiece);
		static bool isBlackPiece(char chPiece);
                static string describePiece(char chPiece);

		enum PieceColour
		{
			WHITE_PIECE=0,
			BLACK_PIECE=1
		};
		enum Player
		{
			WHITE_PLAYER=0,
			BLACK_PLAYER=1
		};
		enum Direction
		{
			HORIZONTAL=0,
			VERTICAL,
			DIAGONAL,
			L_SHAPE
		};
		struct Position
		{
			int iRow;
			int iColumn;
		};
		struct EnPassant
   		{
      			bool bApplied;
      			Position PawnCaptured;
   		};
   		struct Castling
   		{
      			bool bApplied;
     			Position rook_before;
      			Position rook_after;
   		};

   		struct Promotion
   		{
      			bool bApplied;
      			//Position  pos;
      			char chBefore;
      			char chAfter;
   		};

		 struct IntendedMove
   		{
      			char chPiece;
      			Position from;
      			Position to;
   		};

		const char initial_board[8][8] =
   		{
      		// This represents the pieces on the board.
      		// Keep in mind that pieces[0][0] represents A1
      		// pieces[1][1] represents B2 and so on.
      		// Letters in CAPITAL are white
      		{ 'R',  'N',  'B',  'Q',  'K',  'B',  'N',  'R' },
      		{ 'P',  'P',  'P',  'P',  'P',  'P',  'P',  'P' },
      		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
      		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
      		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
      		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
      		{ 'p',  'p',  'p',  'p',  'p',  'p',  'p',  'p' },
      		{ 'r',  'n',  'b',  'q',  'k',  'b',  'n',  'r' },
   		};
};

class game : chess
{
	public:

	game();
	~game();
   	void movePiece( Position present, Position future, chess::EnPassant* S_enPassant, chess::Castling* S_castling, chess::Promotion* S_promotion );

	char getPieceAtPosition(int iRow, int iColumn);
	char getPieceAtPosition(Position pos);
	char getPiece_considerMove( int iRow, int iColumn, IntendedMove* intended_move = nullptr );

	bool isReachable( int iRow, int iColumn, int iColour );

   	bool isSquareOccupied( int iRow, int iColumn );

   	bool isPathFree( Position startingPos, Position finishingPos, int iDirection ); 

   	bool canBeBlocked( Position startingPos, Position finishinPos, int iDirection );

   	bool isCheckMate();

   	bool isKingInCheck( int iColour, IntendedMove* intended_move = nullptr );

   	bool playerKingInCheck( IntendedMove* intended_move = nullptr );

   	bool wouldKingBeInCheck( char chPiece, Position present, Position future, EnPassant* S_enPassant );

   	Position findKing( int iColour );

	void parseMove( string move, Position* pFrom, Position* pTo, char* chPromoted = nullptr );

   	void logMove( string &to_record );
        string getLastMove( void );

        void deleteLastMove( void );
	struct Round
   	{
		string white_move;
		string black_move;
   	};

        deque<Round> rounds;

	vector<char> white_captured;
   	vector<char> black_captured;

	private:

   	// Represent the pieces in the board
   	char board[8][8];
	struct Undo
        {
            bool bCanUndo;
            bool bCapturedLastMove;

            bool bCastlingKingSideAllowed;
            bool bCastlingQueenSideAllowed;

            EnPassant en_passant;
            Castling  castling;
            Promotion promotion;
        } m_undo;
        
        int m_CurrentTurn;
	int m_bGameFinished;
};

