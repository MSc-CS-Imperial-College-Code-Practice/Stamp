enum MessageStatus {
  INVALID_HEADER=-1,
  WRONG_RECIPIENT=-2,
  INVALID_MESSAGE_DIGEST=-3,
  INVALID_HEADER_DIGEST=-4,
  VALID_EMAIL=1
};

/* pre-supplied helper function */
/* generates the SHA1 hash of input string text into output parameter digest */
void text_to_SHA1_digest(const char* text, char* digest);


// I) MAIN FUNCTIONS
int leading_zeros(const char* digest);
bool file_to_SHA1_digest(const char* filename, char* digest);
bool make_header(const char* recipient, const char* filename, char* header);
enum MessageStatus check_header(const char* email_address, const char* header, 
  const char* filename);

// II) AUXILIARY FUNCTIONS

int lenght_txt_file(const char* filename);
void txt_file_to_array(const char* filename, char* array);
void clean_char_array(char *array);
void split_string(const char* input_string, const char* delimiters,
  char* output_array[]);
bool is_valid_email(const char* email);
bool is_valid_digest(const char* digest);
bool is_valid_counting(const char* counter_str);