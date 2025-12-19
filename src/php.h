
typedef struct _zend_resource {
	uint32_t gc[2];	//zend_refcounted_h
	zend_long         handle; // TODO: may be removed ???
	int               type;
	void             *ptr;
} zend_resource;
typedef struct
{
	zend_resource    *res;
	uint32_t type_info;
	uint32_t num_args; /* arguments number for EX(This) */
} zval;

typedef struct _zend_execute_data zend_execute_data;

struct _zend_execute_data
{
	const void *opline;		 /* zend_op                */
	zend_execute_data *call; /* current call                   */
	zval *return_value;
	void *func; /* zend_function              */
	zval This;
	zend_execute_data *prev_execute_data;
	void *symbol_table;//zend_array
	void **run_time_cache; /* cache op_array->run_time_cache */
	void *extra_named_params;//zend_array
};

typedef struct
{
	zval uninitialized_zval;
	zval error_zval;
	void *symtable_cache[__SYMTABLE_CACHE_SIZE__];//zend_array
	void **symtable_cache_limit;//zend_array
	void **symtable_cache_ptr;//zend_array
	char symbol_table[__ZEND_ARRAY_SIZE__];//zend_array
	char included_files[__ZEND_ARRAY_SIZE__];//zend_array
	void *bailout; // JMP_BUF
	int error_reporting;
	__PHP85_EG_FEILDS__
	int exit_status;

	void *function_table; //zend_array
	void *class_table;//zend_array
	void *zend_constants;//zend_array

	zval *vm_stack_top;
	zval *vm_stack_end;
	void* vm_stack;//zend_vm_stack, typedef struct _zend_vm_stack *zend_vm_stack;
	size_t vm_stack_page_size;

	void *current_execute_data;
	void *fake_scope;
	/* Other member fields are omitted .... */
	/* ....... */
} zend_executor_globals;
typedef struct _IO_FILE FILE;
typedef struct _php_stream  {
	const void *ops;
	void *abstract;
} php_stream;
typedef struct {
	FILE *file;
	int fd;
} php_stdio_stream_data;
typedef struct {
	int php_sock;
} php_netstream_data;

typedef unsigned short wchar_t;
char *php_win32_error_to_msg(unsigned long error);
uint8_t php_win32_cp_use_unicode(void);
wchar_t *php_win32_cp_conv_ascii_to_w(const char* in, size_t in_len, size_t *out_len);
wchar_t *php_win32_cp_conv_utf8_to_w(const char* in, size_t in_len, size_t *out_len);
wchar_t *php_win32_cp_conv_cur_to_w(const char* in, size_t in_len, size_t *out_len);