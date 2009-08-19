/* DST  -  Dune 2 Script Tools
 *  
 * Copyright (C) 2009 segra		<segra@strobs.com>
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 * 
 * $Id: scriptHandlerCompiler.h 21 2009-04-11 01:11:32Z segra $
 * 
 */

namespace script {

	class _scriptHandlerCompiler : public _scriptHandler {
	private:
		ifstream		*_sourceFile;				// Source-Code file	

		string			 _currentLine, _opcodeCurrent, _data;
		word			 _opcode;

		bool			 headerCreate();			// Insert the header
		bool		     scriptCompile( );			// Compile to the byte code
		bool			 scriptSave();				// Save the buffer to disk
		int				 scriptSectionCheck();		// Check for section name in _currentLine

	public:
						 _scriptHandlerCompiler( const char *fileName );
						~_scriptHandlerCompiler();

		bool			 execute();							// Compile a script

	private:
		// Opcodes
		void			 o_goto				(   );					
		void			 o_setreturn		(   );
		void			 o_pushOp			(   );
		void			 o_push				(   );
		void			 o_pushWord  		(   );
		void			 o_pushreg			(   );
		void			 o_pushframeMinArg	(   );
		void			 o_pushframePluArg	(   );
		void			 o_popret			(   );
		void			 o_popreg			(   );
		void			 o_popframeMinArg	(   );
		void			 o_popframePluArg	(   );
		void			 o_spadd			(   );
		void			 o_spsub			(   );
		void			 o_execute			(   );
		void			 o_ifnotgoto		(   );
		void			 o_negate			(   );
		void			 o_evaluate			(   );
		void			 o_return			(   );

		void			 o_evaluate_IfEither( 	);
		void			 o_evaluate_Equal(   );
		void			 o_evaluate_NotEqual(   );
		void			 o_evaluate_CompareGreaterEqual(   );
		void			 o_evaluate_CompareGreater(   );
		void			 o_evaluate_CompareLessEqual(   );
		void			 o_evaluate_CompareLess(   );
		void			 o_evaluate_Add(   );
		void			 o_evaluate_Subtract(   );
		void			 o_evaluate_Multiply(   );
		void			 o_evaluate_Divide(   );
		void			 o_evaluate_ShiftRight(   );
		void			 o_evaluate_ShiftLeft(   );
		void			 o_evaluate_And(   );
		void			 o_evaluate_Or(   );
		void			 o_evaluate_DivideRemainder(   );
		void			 o_evaluate_XOR(   );

		inline void	o_execute_Unit_Null(  ) {

		}

		inline void	o_execute_House_Null(  ) {
			
		}
		
		inline void	o_execute_Building_Null(  ) {

		}

		void o_execute_Unit_GetDetail(  );
	};

}
