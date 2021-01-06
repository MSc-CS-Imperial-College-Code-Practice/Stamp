#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include <regex> 
#include "stamp.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

// helper function for internal use only
// transforms raw binary hash value into human-friendly hexademical form
void convert_hash(const unsigned char *str, char *output, int hash_length) {
  char append[16];
  strcpy (output, "");
  for (int n=0; n<hash_length; n++) {
    sprintf(append,"%02x",str[n]);
    strcat(output, append);
  }
}

// pre-supplied helper function
// generates the SHA1 hash of input string text into output parameter digest
// ********************** IMPORTANT **************************
// ---> remember to include -lcrypto in your linking step <---
// ---> so that the definition of the function SHA1 is    <---
// ---> included in your program                          <---
// ***********************************************************
void text_to_SHA1_digest(const char *text, char *digest) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1( (const unsigned char *) text, strlen(text), hash);
  convert_hash(hash, digest, SHA_DIGEST_LENGTH);
}

/* add your function definitions here */

// I) MAIN FUNCTIONS

int leading_zeros(const char* digest){

  int counter = 0;
  for(int i=0; digest[i]!='\0'; i++){
    if(isxdigit(digest[i])==0) // 0 means not valid hexadecimal value
      return -1;
    if(digest[i]=='0' && counter==i)
      counter++;
  }
  return counter;
}

bool file_to_SHA1_digest(const char* filename, char* digest){

  // Cleaning
  clean_char_array(digest);

  // Auxiliar variables
  int text_length = lenght_txt_file(filename);
  if(text_length == -1) // no message text
    return false;
  char text[text_length];

  // Get digest
  txt_file_to_array(filename,text);
  text_to_SHA1_digest(text,digest);
  
  return true;
}

bool make_header(const char* recipient, const char* filename, char* header){

  int counter = 0;
  char counter_str[512];
  char file_digest[41];
  char header_digest[41];

  // Get digest of file. If filename cannot be readen, return false
  if(!file_to_SHA1_digest(filename, file_digest)) 
    return false; 
  
  while(counter <= 1e7){ // 1e7 = 10'000'000 (scientific notation)
    
    // Convert integer counter to CString
    sprintf (counter_str, "%i", counter);
    // Get header for email
    sprintf(header,"%s:%s:%s",recipient,file_digest,counter_str);
    // Get header digest
    text_to_SHA1_digest(header,header_digest);
    // Check if header digest has 5 leading zeros. If yes, return true
    if(leading_zeros(header_digest) == 5)
      return true;
    counter++;
  } 
  
  // No valid header was founded. Return false
  return false;
  
}

enum MessageStatus check_header(const char* email_address, const char* header, 
  const char* filename){

  char *header_inputs[3];
  char computed_header[512];
  char computed_message_digest[41];
  char computed_header_digest_1[41]; // from email_address and email_address
  char computed_header_digest_2[41]; // from header
  

  split_string(header,":",header_inputs);
  
  file_to_SHA1_digest(filename,computed_message_digest);
  make_header(email_address,filename,computed_header);
  text_to_SHA1_digest(computed_header,computed_header_digest_1);
  text_to_SHA1_digest(header,computed_header_digest_2);

  if(is_valid_digest(header_inputs[1]))
    cout << header_inputs[1] << " is a valid digest" << endl;
  else
    cout << header_inputs[1] << " is not a valid digest" << endl;

  // Check INVALID_HEADER
  if(!is_valid_email(header_inputs[0]) || 
     !is_valid_digest(header_inputs[1]) || 
     !is_valid_counting(header_inputs[2]))
    return INVALID_HEADER;
  // Check WRONG_RECIPIENT
  if(strcmp(header_inputs[0], email_address)!=0)
    return WRONG_RECIPIENT;
  // Check INVALID_MESSAGE_DIGEST
  if(strcmp(header_inputs[1],computed_message_digest)!=0)
    return INVALID_MESSAGE_DIGEST;
  // Check INVALID_HEADER_DIGEST
  if(strcmp(computed_header_digest_1,computed_header_digest_2)!=0)
    return INVALID_HEADER_DIGEST;

return VALID_EMAIL;
}



// II) AUXILIARY FUNCTIONS
int lenght_txt_file(const char* filename){

  std::ifstream in;
  int counter = 0;
  char character;
  in.open(filename);
  if (!in)
    return -1;
  while(!in.eof()){
    in.get(character);
    counter++;
  }
  in.close();
  return counter;  

}

void txt_file_to_array(const char* filename, char* array){

  std::ifstream in;
  int index = 0;
  char character;
  in.open(filename);
  if (!in)
    return;
  while(!in.eof()){
    in.get(character);
    array[index] = character;
    index++;
  }
  array[index-1] = '\0'; // Add centinel character to last char
  in.close();

}

void clean_char_array(char *array){
  for(int i=0; array[i]!='\0'; i++){
    array[i] = '\0';
  }
}

void split_string(const char* input_string, const char* delimiters,
  char* output_array[]){
  
  char copied_input_string[512];
  int index = 0;
  strcpy(copied_input_string,input_string); //strtok modifies input string
                                            //copy is created

  // Extract the first token
  char* token = strtok(copied_input_string, delimiters);
  // Loop through the string to extract all other tokens
  while(token != '\0') {
    output_array[index] = new char[512];
    strcpy(output_array[index],token);
    token = strtok(NULL, delimiters);
    index++;
  }
}

bool is_valid_email(const char* email){ 

  // Regular expression definition 
  const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
  // Check if there is a match in the pattern 
  if(regex_match(email, pattern))
    return true;
  return false; 
 
}

bool is_valid_digest(const char* digest){ 

  int digest_length = strlen(digest);
  // Check Lenght
  if(digest_length != 40)
    return false;
  // Check for hexadecimal values
  for(int i=0; digest[i]!='\0'; i++){
    if(isxdigit(digest[i])==0) // 0 means not valid hexadecimal value
      return false;
  }
  return true; 
}
bool is_valid_counting(const char* counter_str){
  int counter = atoi(counter_str);
  if(counter >= 0 && counter <=1e7)
    return true;
  return false;
}