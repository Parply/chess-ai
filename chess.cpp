#pragma once

class chess
{
	public:
		static int getPieceColour(char chPiece);
		static bool isWhitePiece(char chPiece);
		static bool isBlackPiece(char chPiece);

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
			int iColumn
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
