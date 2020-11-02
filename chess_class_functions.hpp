# pragma once
#include "chess_class.hpp"

int chess::getPieceColour(char chPiece)
{
   if (isupper(chPiece))
   {
      return WHITE_PIECE;
   }
   else
   {
      return BLACK_PIECE;
   }
}
bool chess::isWhitePiece(char chPiece)
{
   return getPieceColour(chPiece) == chess::WHITE_PIECE ? true : false;
}

bool chess::isBlackPiece(char chPiece)
{
   return getPieceColour(chPiece) == chess::BLACK_PIECE ? true : false;
}

std::string chess::describePiece(char chPiece)
   {
      std::string description;

      if (isWhitePiece(chPiece))
      {
         description += "White ";
      }
      else
      {
         description += "Black ";
      }

      switch (toupper(chPiece))
      {
         case 'P':
         {
            description += "pawn";
         }
         break;

         case 'N':
         {
            description += "knight";
         }
         break;

         case 'B':
         {
            description += "bishop";
         }
         break;

         case 'R':
         {
            description += "rook";
         }
         break;

         case 'Q':
         {
            description += "queen";
         }
         break;

         default:
         {
            description += "unknow piece";
         }
         break;
      }

      return description;
}

// -------------------------------------------------------------------
// Game class
// -------------------------------------------------------------------
game::game()
{
   // White player always starts
   m_CurrentTurn = WHITE_PLAYER;

   // Game on!
   m_bGameFinished = false;

   // Nothing has happend yet
   m_undo.bCapturedLastMove         = false;
   m_undo.bCanUndo                  = false;
   m_undo.bCastlingKingSideAllowed  = false;
   m_undo.bCastlingQueenSideAllowed = false;
   m_undo.en_passant.bApplied       = false;
   m_undo.castling.bApplied         = false;

   // Initial board settings
   memcpy(board, initial_board, sizeof(char) * 8 * 8);

}

game::~game()
{
   white_captured.clear();
   black_captured.clear();
   rounds.clear();
}

void Game::movePiece(Position present, Position future, Chess::EnPassant* S_enPassant, Chess::Castling* S_castling, Chess::Promotion* S_promotion)
{
   // Get the piece to be moved
   char chPiece = getPieceAtPosition(present);

   // Is the destination square occupied?
   char chCapturedPiece = getPieceAtPosition(future);

   // So, was a piece captured in this move?
   if (0x20 != chCapturedPiece)
   {
      if (WHITE_PIECE == getPieceColor(chCapturedPiece))
      {
         // A white piece was captured
         white_captured.push_back(chCapturedPiece);
      }
      else
      {
         // A black piece was captured
         black_captured.push_back(chCapturedPiece);
      }

      // Set Undo structure. If a piece was captured, then no "en passant" move performed
      m_undo.bCapturedLastMove = true;
      
      // Reset m_undo.castling
      memset( &m_undo.en_passant, 0, sizeof( Chess::EnPassant ));
   }
   else if (true == S_enPassant->bApplied)
   {
      char chCapturedEP = getPieceAtPosition(S_enPassant->PawnCaptured.iRow, S_enPassant->PawnCaptured.iColumn);

      if (WHITE_PIECE == getPieceColor(chCapturedEP))
      {
         // A white piece was captured
         white_captured.push_back(chCapturedEP);
      }
      else
      {
         // A black piece was captured
         black_captured.push_back(chCapturedEP);
      }

      // Now, remove the captured pawn
      board[S_enPassant->PawnCaptured.iRow][S_enPassant->PawnCaptured.iColumn] = EMPTY_SQUARE;

      // Set Undo structure as piece was captured and "en passant" move was performed
      m_undo.bCapturedLastMove = true;
      memcpy(&m_undo.en_passant, S_enPassant, sizeof(Chess::EnPassant));
   }
   else
   {
      m_undo.bCapturedLastMove   = false;
      
      // Reset m_undo.castling
      memset( &m_undo.en_passant, 0, sizeof( Chess::EnPassant ));
   }

   // Remove piece from present position
   board[present.iRow][present.iColumn] = EMPTY_SQUARE;

   // Move piece to new position
   if ( true == S_promotion->bApplied )
   {
      board[future.iRow][future.iColumn] = S_promotion->chAfter;

      // Set Undo structure as a promotion occured
      memcpy(&m_undo.promotion, S_promotion, sizeof(Chess::Promotion));
   }
   else
   {
      board[future.iRow][future.iColumn] = chPiece;

      // Reset m_undo.promotion
      memset( &m_undo.promotion, 0, sizeof( Chess::Promotion ));
   }  

   // Was it a castling move?
   if ( S_castling->bApplied == true  )
   {
      // The king was already move, but we still have to move the rook to 'jump' the king
      char chPiece = getPieceAtPosition(S_castling->rook_before.iRow, S_castling->rook_before.iColumn);

      // Remove the rook from present position
      board[S_castling->rook_before.iRow][S_castling->rook_before.iColumn] = EMPTY_SQUARE;

      // 'Jump' into to new position
      board[S_castling->rook_after.iRow][S_castling->rook_after.iColumn] = chPiece;

      // Write this information to the m_undo struct
      memcpy(&m_undo.castling, S_castling, sizeof(Chess::Castling));

      // Save the 'CastlingAllowed' information in case the move is undone
      m_undo.bCastlingKingSideAllowed  = m_bCastlingKingSideAllowed[getCurrentTurn()] ;
      m_undo.bCastlingQueenSideAllowed = m_bCastlingQueenSideAllowed[getCurrentTurn()];
   }
   else
   {
      // Reset m_undo.castling
      memset( &m_undo.castling, 0, sizeof( Chess::Castling ));
   }

   // Castling requirements
   if ( 'K' == toupper(chPiece) )
   {
      // After the king has moved once, no more castling allowed
      m_bCastlingKingSideAllowed[getCurrentTurn()]  = false;
      m_bCastlingQueenSideAllowed[getCurrentTurn()] = false;
   }
   else if ('R' == toupper(chPiece) )
   {
      // If the rook moved from column 'A', no more castling allowed on the queen side
      if ( 0 == present.iColumn )
      {
         m_bCastlingQueenSideAllowed[getCurrentTurn()] = false;
      }

      // If the rook moved from column 'A', no more castling allowed on the queen side
      else if ( 7 == present.iColumn )
      {
         m_bCastlingKingSideAllowed[getCurrentTurn()] = false;
      }
   }

   // Change turns
   changeTurns();

   // This move can be undone
   m_undo.bCanUndo = true;
}

void game::undoLastMove()
{
   string last_move = getLastMove();

   // Parse the line
   chess::Position from;
   chess::Position to;
   parseMove(last_move, &from, &to);

   // Since we want to undo a move, we will be moving the piece from (iToRow, iToColumn) to (iFromRow, iFromColumn)
   char chPiece = getPieceAtPosition(to.iRow, to.iColumn);

   // Moving it back
   // If there was a castling
   if ( true == m_undo.promotion.bApplied )
   {
      board[from.iRow][from.iColumn] = m_undo.promotion.chBefore;
   }
   else
   {
      board[from.iRow][from.iColumn] = chPiece;
   }

   // Change turns
   changeTurns();

   // If a piece was captured, move it back to the board
   if (m_undo.bCapturedLastMove)
   {
      // Let's retrieve the last captured piece
      char chCaptured;

      // Since we already changed turns back, it means we should we pop a piece from the oponents vector
      if (WHITE_PLAYER == m_CurrentTurn)
      {
         chCaptured = black_captured.back();
         black_captured.pop_back();
      }
      else
      {
         chCaptured = white_captured.back();
         white_captured.pop_back();
      }

      // Move the captured piece back. Was this an "en passant" move?
      if (m_undo.en_passant.bApplied)
      {
         // Move the captured piece back
         board[m_undo.en_passant.PawnCaptured.iRow][m_undo.en_passant.PawnCaptured.iColumn] = chCaptured;

         // Remove the attacker
         board[to.iRow][to.iColumn] = EMPTY_SQUARE;
      }
      else
      {
         board[to.iRow][to.iColumn] = chCaptured;
      }
   }
   else
   {
      board[to.iRow][to.iColumn] = EMPTY_SQUARE;
   }

   // If there was a castling
   if ( m_undo.castling.bApplied )
   {
      char chRook = getPieceAtPosition(m_undo.castling.rook_after.iRow, m_undo.castling.rook_after.iColumn);

      // Remove the rook from present position
      board[m_undo.castling.rook_after.iRow][m_undo.castling.rook_after.iColumn] = EMPTY_SQUARE;

      // 'Jump' into to new position
      board[m_undo.castling.rook_before.iRow][m_undo.castling.rook_before.iColumn] = chRook;

      // Restore the values of castling allowed or not
      m_bCastlingKingSideAllowed[getCurrentTurn()]  = m_undo.bCastlingKingSideAllowed;
      m_bCastlingQueenSideAllowed[getCurrentTurn()] = m_undo.bCastlingQueenSideAllowed;
   }

   // Clean m_undo struct
   m_undo.bCanUndo             = false;
   m_undo.bCapturedLastMove    = false;
   m_undo.en_passant.bApplied  = false;
   m_undo.castling.bApplied    = false;
   m_undo.promotion.bApplied   = false;

   // If it was a checkmate, toggle back to game not finished
   m_bGameFinished = false;

   // Finally, remove the last move from the list
   deleteLastMove();
}

bool game::undoIsPossible()
{
   return m_undo.bCanUndo;
}

bool game::castlingAllowed(Side iSide, int iColor)
{
   if ( QUEEN_SIDE == iSide )
   {
      return m_bCastlingQueenSideAllowed[iColor];
   }
   else //if ( KING_SIDE == iSide )
   {
      return m_bCastlingKingSideAllowed[iColor];
   }
}

char game::getPieceAtPosition(int iRow, int iColumn)
{
   return board[iRow][iColumn];
}

char game::getPieceAtPosition(Position pos)
{
   return board[pos.iRow][pos.iColumn];
}
char game::getPiece_considerMove(int iRow, int iColumn, IntendedMove* intended_move)
{
   char chPiece;

   // If there is no intended move, just return the current position of the board
   if (nullptr == intended_move)
   {
      chPiece = getPieceAtPosition(iRow, iColumn);
   }
   else
   {
      // In this case, we are trying to understand what WOULD happed if the move was made,
      // so we consider a move that has not been made yet
      if (intended_move->from.iRow == iRow && intended_move->from.iColumn == iColumn)
      {
         // The piece wants to move from that square, so it would be empty
         chPiece = EMPTY_SQUARE;
      }
      else if (intended_move->to.iRow == iRow && intended_move->to.iColumn == iColumn)
      {
         // The piece wants to move to that square, so return the piece
         chPiece = intended_move->chPiece;
      }
      else
      {
         chPiece = getPieceAtPosition(iRow, iColumn);
      }
   }

   return chPiece;
}


