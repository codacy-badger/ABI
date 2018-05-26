#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/abi_bf.h"
//#include "../include/abi_cmt.h"


//!< command line parameter
#define STR_PARAM  "f:"

#define ABI_MEM_SIZE 30000

#define MAX_LINE_BUF 1024

int main(int argc, char * argv[]){

	int c = 0;  //!< opt character
	/*  \brief mode flag
	 *  \enum MODE_SCRIPT interpret a brainfuck script file
	 *  \enum MODE_INTERACTIVE run the brainfuck interpreter in interactive mode 
	 * */
	enum {MODE_SCRIPT,MODE_INTERACTIVE} mode = MODE_INTERACTIVE;

	char * file = NULL;  //!< script file

	//!< handle command line parameters
	while((c = getopt(argc,argv,STR_PARAM)) != -1){
		switch(c){
			case 'f':
				//!< interpret brainfuck script
				if(access(optarg, F_OK) != -1){
					mode = MODE_SCRIPT;
					file = optarg;
				}else{
					fprintf(stderr,"Err:unexist file %s!\n",optarg);
					abort();
				}
				break;
			case '?':
				if(optopt == 'f')
					fprintf(stderr,"Option -%c requires an argument.\n",optopt);
				else if(isprint(optopt))
					fprintf(stderr,"Unkown option `-%c`.\n",optopt);
				else
					fprintf(stderr,"Unkown option character `\\x%x`.\n",optopt);
				break;
			default:
				abort();
				break;
		}
	}       

	//!< select script input file to `i_stream`
	if(MODE_SCRIPT == mode){   

		bf_ast_t * ast = bf_ast_new(bf_instruction_interpreter);
		bf_ast_init_4_script(ast, file);
		//bf_ast_dfs_pre(ast);

		bf_context_t * context = bf_context_new(ABI_MEM_SIZE);

		/*
		bf_ast_node_t * tail = bf_ast_tail(ast->root);
		if(NULL != tail)
			fprintf(stderr, "tail->ins->token == `%c`\n", 
					tail->instruction->token);
		*/
		
		if(bf_ast_executable(ast))
			bf_execute(context, ast);

		bf_context_release(context);
		bf_ast_release(ast);
	}else if(MODE_INTERACTIVE == mode){

		char buf[MAX_LINE_BUF] = {0};
		printf(">>>");
		fgets(buf, sizeof(buf), stdin);

		bf_ast_t * ast = bf_ast_new(bf_instruction_interpreter);
		bf_ast_init_4_string(ast, buf);
		bf_ast_dfs_pre(ast);

		bf_context_t * context = bf_context_new(ABI_MEM_SIZE);

		if(bf_ast_executable(ast))
			bf_execute(context, ast);

		bf_context_release(context);
		bf_ast_release(ast);
	}else{
		fprintf(stderr,"Unkown mode!\n");
		abort();
	}

	//!< handle brainfuck 
	//abi_f(i_stream);

	//!< close file 
	//fclose(i_stream);

	//printf("ok!\n");
	return 0;
}

/* ******************** brainfuck interpreter ******************** */


/*  \brief append a byte to buffer
 *  \param buf buffer to append
 *  \param max max size of buffer
 *  \param byte byte appended to buffer
 *  \retval 0 for ok,-1 for fail
 * */
/*
   static void append(struct buffer buf, size_t max, uint8_t byte){
   if(buf.len_valid < max){  //!< buf.buf[max_length] == '\0'
   buf.buf[buf.len_valid] = byte;
   buf.len_valid++;
   }else{
   fprintf(stderr,"Out of buffer in `%s`.\n",__FUNCTION__);
   abort();
   }
   }
   */

