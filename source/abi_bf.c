//#define NDEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../include/abi_bf.h"

/*! \brief create a instruction
 *  \param token token character
 *  \param count count of token countinue
 *  \retval created brainfuck instruction
 * */
bf_instruction_t * bf_instruction_new(char token, int count){
	bf_instruction_t * instruction = malloc(sizeof(bf_instruction_t));
	assert(NULL != instruction);
	if(NULL == instruction)
		return NULL;

	instruction->count = count;
	instruction->token = token;
	
	return instruction;
}

/*! \brief release brainfuck instruction
 * */
void bf_instruction_release(bf_instruction_t * instruction){
	free(instruction);
}

/*! \brief create a brainfuck AST node
 *  \param instruction the instruction of AST node
 *  \retval created brainfuck AST node
 * */
bf_ast_node_t * bf_ast_node_new(bf_instruction_t * instruction){
	bf_ast_node_t * node = malloc(sizeof(bf_ast_node_t));
	assert(NULL != instruction && NULL != node);
	if(NULL == instruction || NULL == node)
		return NULL;

	node->instruction = instruction;
	node->loop = NULL;
	node->next = NULL;

	return node;
}

/*! \brief release the brainfuck AST node
 * */
void bf_ast_node_release(bf_ast_node_t * node){
	free(node->instruction);
	free(node);
}

/*! \brief create a brainfuck AST 
 *  \param interpreter interpreter of each brainfuck instruction
 *  \retval created brainfuck AST
 * */
bf_ast_t * bf_ast_new(bf_ast_instruction_interpreter_t interpreter){
	bf_ast_t * ast = malloc(sizeof(bf_ast_t));
	assert(NULL != interpreter && NULL != ast);
	if(NULL == interpreter || NULL ==ast)
		return NULL;

	ast->interpreter = interpreter;
	ast->root = NULL;

	return ast;
}

static void bf_ast_release_post(bf_ast_node_t * root){
	//assert(NULL != root);
	if(NULL == root)
		return ;

	bf_ast_release_post(root->loop);
	bf_ast_release_post(root->next);

	bf_instruction_release(root->instruction);
	free(root);
}

/*! \brief release a brainfuck AST
 *  \param AST to release
 * */
void bf_ast_release(bf_ast_t * ast){
	assert(NULL != ast);
	if(NULL == ast)
		return;

	//!< release tree
	bf_ast_release_post(ast->root);

	//!< release handle
	free(ast);
}

/*! \brief add node to a brainfuck AST by token
 *  \param ast instance of brainfuck AST
 *  \param token token of brainfuck instruction
 * */
//void bf_ast_node_add(bf_ast_t * ast, char token);

/*! \brief initialize brainfuck AST from script file
 *  \param ast brainfuck AST instance
 *  \param file brainfuck script file name
 * */
static bf_ast_node_t * bf_ast_init_4_stream(FILE * stream){
	assert(NULL != stream);
	if(NULL == stream)
		return NULL;

	char c = 0;
	bf_ast_node_t * root = bf_ast_node_new(bf_instruction_new('\0', 0));
	bf_ast_node_t * node = root;

	while((c = fgetc(stream)) != EOF){
		switch(c){
			case BF_TOKEN_MEM_ITEM_INC:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_ITEM_INC;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_ITEM_INC);
				ungetc(c, stream);
				break;
			case BF_TOKEN_MEM_ITEM_DEC:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_ITEM_DEC;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_ITEM_DEC);
				ungetc(c, stream);
				break;
			case BF_TOKEN_MEM_PTR_INC:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_PTR_INC;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_PTR_INC);
				ungetc(c, stream);
				break;
			case BF_TOKEN_MEM_PTR_DEC:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_PTR_DEC;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_PTR_DEC);
				ungetc(c, stream);
				break;
			case BF_TOKEN_MEM_ITEM_OUTPUT:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_ITEM_OUTPUT;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_ITEM_OUTPUT);
				ungetc(c, stream);
				break;
			case BF_TOKEN_MEM_ITEM_INPUT:
				do{
					node->instruction->token = 
						BF_TOKEN_MEM_ITEM_INPUT;
					node->instruction->count ++;
				
				}while((c = fgetc(stream)) == BF_TOKEN_MEM_ITEM_INPUT);
				ungetc(c, stream);
				break;
			case BF_TOKEN_CTL_LOOP_START:
				node->instruction->token = 
					BF_TOKEN_CTL_LOOP_START;
				node->instruction->count = 1;
				//< build loop branch
				node->loop = bf_ast_init_4_stream(stream);
				break;
			case BF_TOKEN_CTL_LOOP_END:
				//< a ast node with invalid instruction
				//< and without children
				return root;
				break;
			//default:
				//fprintf(stderr, "useless token `%c`\n",c);
		}  //!< end of switch(token)

		//< initialize and scroll to new node
		node->next = bf_ast_node_new(bf_instruction_new('\0', 0));
		node = node->next;

	}  //!< end of while(! EOF)
	
	return root;
}


void bf_ast_init_4_script(bf_ast_t * ast, const char * script){
	assert(NULL != ast && NULL != script);
	if(NULL == ast || NULL == script)
		return;

	FILE * stream = fopen(script, "r");
	assert(NULL != stream);

	if(NULL != stream){
		ast->root = bf_ast_init_4_stream(stream);
		fclose(stream);
	}
}

/*! \brief create a context of brainfuck interpreter runtime
 *  \param mem_size size of brianfuck interpreter 
 *  \param context of a interpreter
 * */
bf_context_t * bf_context_new(size_t mem_size){
	uint8_t * mem = malloc(mem_size);
	bf_context_t * context = malloc(sizeof(bf_context_t));
	assert(NULL != mem && NULL != context);
	if(NULL == mem || NULL == context)
		return NULL;

	context->mem_ptr = mem;
	context->mem_size = mem_size;
	context->mem_index = 0;

	return context;
}

/*! \brief release a context
 *  \parma context to release
 * */
void bf_context_release(bf_context_t * context){
	assert(NULL != context);
	if(NULL == context)
		return;

	free(context->mem_ptr);
	free(context);
}

static void _bf_ast_dfs(bf_ast_node_t * root){
	if(NULL == root)
		return;

	//fprintf(stderr, "token-`%c`, count-`%d`.\n", root->instruction->token,
			//root->instruction->count);
	for(int i=0; i<root->instruction->count; i++)
		fputc(root->instruction->token, stderr);

	_bf_ast_dfs(root->loop);
	_bf_ast_dfs(root->next);
}

void bf_ast_dfs(bf_ast_t * ast){
	assert(NULL != ast);
	if(NULL == ast)
		return;

	_bf_ast_dfs(ast->root);
}

/*! \brief execute brainfuck AST by variant pre-order traversal in specify context
 *  \param context current runtime context 
 *  \param ast brainfuck AST to execute
 * */
static void bf_ast_execute(bf_context_t * context, bf_ast_t * ast, bf_ast_node_t * root){
	assert(NULL != context && NULL != ast && NULL != ast->interpreter);
	if(NULL == context || NULL == ast || NULL == ast->interpreter)
		return;

	//< execute instruction
	if(NULL == root)
		return;

	//< execute instruction(excipt control instruction)
	ast->interpreter(context, root->instruction);
#ifndef NDEBUG
	//< trace instruction stream
	fputc(root->instruction->token, stderr);
#endif

	//< pre-order traversal
	//< loop until point to 0
	while(0 < context->mem_ptr[context->mem_index]){
		bf_ast_execute(context, ast, root->loop);
	}
	bf_ast_execute(context, ast, root->next);
}

/*! \brief execute brainfuck AST by variant pre-order traversal in specify context
 *  \param context current runtime context 
 *  \param ast brainfuck AST to execute
 * */
void bf_execute(bf_context_t * context, bf_ast_t * ast){
	assert(NULL != context && NULL != ast);
	if(NULL == context || NULL == ast)
		return;

	bf_ast_execute(context, ast, ast->root);
}

void bf_instruction_interpreter(bf_context_t * context, bf_instruction_t * instruction){
	assert(NULL != context && NULL != instruction);
	if(NULL == context || NULL == instruction)
		return;

	switch(instruction->token){
		case BF_TOKEN_MEM_ITEM_INC:
			if(context->mem_index < context->mem_size){
				context->mem_ptr[context->mem_index] += instruction->count;
			}else{
				
			}
			break;
		case BF_TOKEN_MEM_ITEM_DEC:
			if(context->mem_index < context->mem_size){
				context->mem_ptr[context->mem_index] -= instruction->count;
			}else{
				
			}
			break;
		case BF_TOKEN_MEM_PTR_INC:
			if(context->mem_size-1 > (context->mem_index + instruction->count))
				context->mem_index += instruction->count;
			break;
		case BF_TOKEN_MEM_PTR_DEC:
			if(0 < (context->mem_index - instruction->count))
				context->mem_index -= instruction->count;
			break;
		case BF_TOKEN_MEM_ITEM_OUTPUT:
			for(int i=0; i<instruction->count; i++){
				putchar(context->mem_ptr[context->mem_index]);
			}
			break;
		case BF_TOKEN_MEM_ITEM_INPUT:
			for(int i=0; i<instruction->count; i++){
				//printf("Please Input char:");
				context->mem_ptr[context->mem_index] = getchar();
			}
			break;
		//< only execute non control instruction
		case BF_TOKEN_CTL_LOOP_START:
			break;
		case BF_TOKEN_CTL_LOOP_END:
			break;
		//default:
			//fprintf(stderr, "useless instruction->token `%c`\n",instruction->token);
	}
}

/// @}


