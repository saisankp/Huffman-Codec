// code for a huffman coder


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "huff.h"

// create a new huffcoder structure
struct huffcoder *  huffcoder_new()
{
  //declare a new huffcoder structure
  struct huffcoder * huffcoder;

  //allocate memory for huffcoder
  huffcoder = malloc(sizeof(struct huffcoder));
  
  //allocate memory for the tree in huffcoder
  huffcoder->tree = malloc(sizeof(struct huffchar));

  //use a for-loop to initialize huffcoder
  for(int i = 0; i < NUM_CHARS; i++) {
    huffcoder->freqs[i] = 0; //all frequencies can be 0.
    huffcoder->codes[i] = malloc(sizeof(char) * NUM_CHARS); //allocated enough space for 256 chars, just be be sure codes[i] will have enough space.
    huffcoder->code_lengths[i] = 0; //all code length's are intialized to 0.
  }
	return huffcoder;
}

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffcoder_count(struct huffcoder * this, char * filename)
{
  //make the char unsigned, generally safer to do so.
  unsigned char c;
  FILE * file;
  
  //use fopen to open the filename pointed to by filename using the given mode, in this case "r".
  //"r" opens a file for reading.
  file = fopen(filename, "r");

  //use fgetc to get the next character (unsigned char) from the file
  c = fgetc(file);


  //now let's count the frequencies of the character!
  while(!feof(file)){ //while we haven't reached the end of the file
    unsigned indexOfFreqs;
    indexOfFreqs = (unsigned) c;
    this->freqs[indexOfFreqs]++;
    //move to next character
    c = fgetc(file);
}
fclose(file);
  
  //before we forget, let's set chars with a zero frequency to one.
	for(int i = 0; i < NUM_CHARS; i++){
		if(this->freqs[i] == 0){
			this->freqs[i] = 1;
		}
	}
}

//find the least frequent character and return it's index.
int find_least_freq(struct huffchar ** list, int list_size) {
  int min = list[0]->freq;
  int min_index = 0;

  //go through the other freqencies and see if we can find a new minimmum frequency.
  for(int i = 1; i < list_size; i++) {
    if(list[i]->freq < min) {
      //we have a new minimum
      min = list[i]->freq;
      min_index = i;
    }
    else if(list[i]->freq == min && list[i]->seqno < list[min_index]->seqno){ //seqno used for tie breaking
      min_index = i;
    }
  }
  //return the index of the least frequent.
  return min_index;
}

//create a new compound and initialize it.
struct huffchar * new_compound(struct huffchar * minimum1, struct huffchar * minimum2, int seqno) {
//create huffchar for the new compound and allocate some memory for it.
struct huffchar * result = malloc(sizeof(struct huffchar)); 
//make is_compound = 1 to make a new compound
result->is_compound = 1;
result->freq = minimum1->freq + minimum2->freq; //the sum of the minimum frequences = frequency of result

//allocate memory for the left and right huffchar
result->u.compound.left = malloc(sizeof(struct huffchar));
result->u.compound.right = malloc(sizeof(struct huffchar));

//set them both equal to the first minimum and the second minimum respectively.
result->u.compound.left = minimum1;
result->u.compound.right = minimum2;

//set the seqno, which was passed into the function.
result->seqno = seqno;
return result;
}




// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffcoder_build_tree(struct huffcoder * this)
{
  //make a huffchar called list with size 256.
  struct huffchar * list[NUM_CHARS];
  int seqno = 256;
  
  //go through the list
  for(int i=0; i<NUM_CHARS; i++) {
    list[i] = malloc(sizeof(struct huffchar)); //allocate memory
		list[i]->freq = this->freqs[i]; //set the frequency 
		list[i]->is_compound = 0; // set is_compound = 0
		list[i]->seqno = i; //used for tie breaking
		list[i]->u.c = i; //set the usigned char
  }

  int list_size = 256;
  while(list_size > 1) {
    int smallest = find_least_freq(list, list_size); //find the index of the smallest.
    struct huffchar * min1 = list[smallest]; //make a huffchar for the first minimum, using the index of the smallest frequency.
    list[smallest] = list[(list_size) - 1]; //decrease the size.
    smallest = find_least_freq(list, ((list_size)-1)); //find the index of the second smallest
    struct huffchar * min2 = list[smallest]; //make a huffchar for the second minimum, using the index of the second smallest frequency.
    list[smallest] = list[(list_size) - 2]; //decrease the size.

    //create compound char using the new_compound function.
    struct huffchar *compound;
    compound = new_compound(min1, min2, seqno); //pass in min1, min2, and the sequence number.

    //now increment seqno, and decrement the size of the list.
    seqno++;
    list_size--;
    list[list_size - 1] = compound;
    this->tree = list[0];

  }
}

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffcoder_tree2table(struct huffcoder * this)
{
  //make an empty char to pass into the recursive function.
  char * empty = malloc(sizeof(char *));
  empty = ""; 
  //also, pass in the tree and the tree height as 0.
  huffcoder_tree2table_recursive(this, this->tree, 0, empty);
}

void huffcoder_tree2table_recursive(struct huffcoder * this, struct huffchar * huffchar, int treeHeight, char * result) {
  
  //if it's not a compound
  if (huffchar->is_compound != 1) {
    this->code_lengths[huffchar->u.c] = treeHeight; //set the code length to the tree height.
    char * y = this->codes[huffchar->u.c]; //get char y equal to the codes
    strcpy(y, result); //copy the string pointed to by result to char y.
  }

  //if it's a compound
  else if(huffchar->is_compound == 1) {
    char x[NUM_CHARS]; 
    strcpy(x, result); //copy the result parameter passed in, into char x of size NUM_CHARS (256)
    strcat(x, "0");  //append the string "0" to the end of x.

    //use recursion, but pass in the left huffchar, height+1, and the value of x.
    huffcoder_tree2table_recursive(this, huffchar->u.compound.left, treeHeight+1, x);

    strcpy(x, result); //copy the result parameter passed in, into char x of size NUM_CHARS (256)
    strcat(x, "1"); //append the string "1" at the end of x.
    
    //use recursion, but pass in the right huffchar, height+1, and the value of x.
    huffcoder_tree2table_recursive(this, huffchar->u.compound.right, treeHeight+1, x);
  }
  
}

// print the Huffman codes for each character in order;
// you should not modify this function
void huffcoder_print_codes(struct huffcoder * this)
{
  for ( int i = 0; i < NUM_CHARS; i++ ) {
    // print the code
    printf("char: %d, freq: %d, code: %s\n", i, this->freqs[i], this->codes[i]);;
  }
}

// encode the input file and write the encoding to the output file
void huffcoder_encode(struct huffcoder * this, char * input_filename,
    char * output_filename)
{
  //open the input file using the given mode, here it is "r" to read.
   FILE * input;
   input = fopen(input_filename, "r");
   assert(input != NULL);

  //open the output file using the given mode, here it is "w" to write.
   FILE * output;
   output = fopen(output_filename, "w");
   assert(output != NULL);
   
  //get the next char
  unsigned char c = fgetc(input);

    while(!feof(input)){ //while we haven't hit the end of the file
       fprintf(output, "%s", this->codes[c]); //output the code as a string
        c = fgetc(input);
  } 
    //NOTE: I noticed an extra end of transmission character (EOT or ASCII value decimal 4) encoded at the end, so let's take care of that to pass the tests.
    fprintf(output, "%s", this->codes[(unsigned char)4]); //to take care of the extra EOT or ASCII value decimal 4
    
    //close the stream
    int closed = fclose(input);
    int closed2 = fclose(output);

    //closed and closed2 should both be 0 if the stream is successfully closed.
    assert(!closed & !closed2);
}

// decode the input file and write the decoding to the output file
void huffcoder_decode(struct huffcoder * this, char * input_filename,
    char * output_filename)
{

  //open the output file using the given mode, here it is "w" to write.
   FILE * output;
   output = fopen(output_filename, "w");
   assert(output != NULL); 

  //open the input file using the given mode, here it is "r" to read.
   FILE * input;
   input = fopen(input_filename, "r");
   assert(input != NULL); 
    
  int done = 0; //let's use this integer to help us know if we hit an EOT.
  struct huffchar * node = this->tree; //let node equal to the root of the tree.

  while(!feof(input) && done == 0) { //while we haven't hit the end of the file, and we haven't hit a EOT.
    char bit = fgetc(input); //get the next bit.

    if(bit == '0') {
      //according to huffman coding, we must go left if the bit is 0.
      node = node->u.compound.left;
    }
    else if(bit == '1') {
       //according to huffman coding, we must go right if the bit is 1.
      node = node->u.compound.right;
    }

    //if we reached a single character
    if(node->is_compound == 0) {

      if(node->u.c == 4) { //this condition is for handling the EOT at the end (with an ASCII value decimal 4)
        done = 1; //finish the while loop!
      }
      else { 
       fprintf(output, "%c", node->u.c); //print the character to the output file.
       node = this->tree; //make the node at the root of the tree again.
      }
    }
  }

   //close the stream
   int closed = fclose(output);
   int closed2 = fclose(input);

   //closed and closed2 should both be 0 if the stream is successfully closed.
   assert(!closed & !closed2);
}
