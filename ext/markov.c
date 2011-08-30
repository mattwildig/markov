#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

static const char* DEFAULT_SENTINEL_WORD = "SENTINEL_WORD";
static const int DEFAULT_PREFIX_LENGTH = 2;
static const int DEFAULT_OUTPUT_WORDS = 250;
static const int DEFAULT_STATE_TABLE_SIZE = 4093; //4093 is from Practice of Programming

typedef struct SuffixNode SuffixNode;
typedef struct StateNode StateNode;
typedef struct MarkovData MarkovData;

struct MarkovData {
    int prefix_len;
    const char* sentinel_word;
    int output_words;
    
    int statetab_len;
    StateNode** statetab;
    
    char initialized;

};

struct SuffixNode {
    const char* suffix;
    SuffixNode* next;
};

struct StateNode {
    const char** prefix;
    SuffixNode* list;
    StateNode* next;
};

static int hash_prefix(MarkovData* d, const char** prefix) {
    unsigned int h = 0;
    
    for (int i = 0; i < d->prefix_len; i++) {
        for (const char* c = prefix[i]; *c != '\0'; c++) {
            h = 31 * h + *c;
        }
    }
    
    return h % d->statetab_len;
}

static bool prefix_match(const char** prefix1, const char** prefix2, int num_words) {
    for (int i = 0; i < num_words; i++) {
        if (strcmp(prefix1[i], prefix2[i]) != 0) {
            return false;
        }
    }
    return true;
}

static StateNode* lookup_state(MarkovData* data, const char** prefix) {
    int h = hash_prefix(data, prefix);
    
    for (StateNode* node = data->statetab[h]; node != NULL; node = node->next) {
        if (prefix_match(node->prefix, prefix, data->prefix_len)) {
            return node;
        }
    }
    return NULL;
}

static void add_suffix(MarkovData* data, const char** prefix, const char* word) {
    StateNode* state_node = lookup_state(data, prefix);
    
    if (state_node == NULL) {
        state_node = malloc(sizeof(StateNode));
        
        state_node->prefix = malloc(sizeof(char*) * data->prefix_len);
        for(int i = 0; i < data->prefix_len; i++)
        {
            state_node->prefix[i] = prefix[i];
        }
        state_node->list = NULL;
        int h = hash_prefix(data, prefix);
        state_node->next = data->statetab[h];
        data->statetab[h] = state_node;
    }
    
    SuffixNode* suffix_node = malloc(sizeof(SuffixNode));
    suffix_node->suffix = word;
    suffix_node->next = state_node->list;
    state_node->list = suffix_node;
}

static void rotate_prefix(MarkovData* data, const char** prefix, const char* word) {
    memmove(prefix, prefix+1, data->prefix_len * sizeof(prefix[0]));
    prefix[data->prefix_len - 1] = word;
}

static void prepopulate_prefix(MarkovData* data, const char** prefix) {
    for (int i = 0; i < data->prefix_len; i++) {
        prefix[i] = data->sentinel_word;
    }
}

static void set_defaults(MarkovData* data) {
    if (data->statetab_len == 0) data->statetab_len = DEFAULT_STATE_TABLE_SIZE;
    data->statetab = malloc(sizeof(StateNode*) * DEFAULT_STATE_TABLE_SIZE);
    
    for (int i = 0; i < data->statetab_len; i++) {
        data->statetab[i] = NULL;
    }
    
    if (data->prefix_len == 0) data->prefix_len = DEFAULT_PREFIX_LENGTH;
    
    if (data->sentinel_word == NULL) data->sentinel_word = strdup(DEFAULT_SENTINEL_WORD);
    
    if (data->output_words == 0) data->output_words = DEFAULT_OUTPUT_WORDS;
    
    data->initialized = 1;
}

void check_data_initialised(MarkovData* data) {
    
    if (! data->initialized) {
        set_defaults(data);
    }
}

static void add_input(MarkovData* data, int (*get_word)(char* /*target buffer*/, char* /*format*/, void* /*data*/), void* input_data){
    check_data_initialised(data);
    
    const int buf_size = 100;  //TODO: handle possibility of words > 100 chars in length
    char buf[buf_size];
    
    char fmt[10];
    sprintf(fmt, "%%%ds", buf_size -1);
    
    const char* prefix[data->prefix_len];
    prepopulate_prefix(data, prefix);
    
    while (get_word(buf, fmt, input_data) != EOF) {
        char* const word = strdup(buf); //TODO: check for errors on strdup
        add_suffix(data, prefix, word);
        rotate_prefix(data, prefix, word);
    }
    add_suffix(data, prefix, strdup(data->sentinel_word));
}

static int get_word_from_stream(char* buffer, char* fmt, void* data){
    FILE* input = (FILE*)data;
    return fscanf(input, fmt, buffer);
}

// static int get_word_from_string(char* buffer, char* fmt, void* data){
//     
// }

void markov_add_input_from_stream(MarkovData* data, FILE* input) {
    add_input(data, get_word_from_stream, input);
}

void markov_add_input_from_string(MarkovData* data, char* string) {
    printf("Adding from string");
    check_data_initialised(data);
    
    const int buf_size = 100;  //TODO: handle possibility of words > 100 chars in length
    char buf[buf_size];

    char fmt[10];
    sprintf(fmt, "%%%ds", buf_size -1);
    
    const char* prefix[data->prefix_len];
    prepopulate_prefix(data, prefix);
    
    while (sscanf(string, fmt, buf) != EOF) {
        char* const word = strdup(buf); //TODO: check for errors on strdup
        add_suffix(data, prefix, word);
        rotate_prefix(data, prefix, word);
        string += strlen(word) +1;
    }
    
    add_suffix(data, prefix, strdup(data->sentinel_word));
}

MarkovData* markov_init() {
    MarkovData* d = malloc(sizeof(MarkovData)); 
    d->statetab_len = 0;
    d->statetab = NULL;
    d->output_words = 0;
    
    d->prefix_len = 0;
    d->sentinel_word = NULL;
    
    //TODO: error checking on mallocs
    
    d->initialized = 0;
    
    return d;
}

void markov_set_table_size(MarkovData* data, int size) {
    if (data->initialized) return;
    data->statetab_len = size;
}

void markov_set_prefix_len(MarkovData* data, int len) {
    if (data->initialized) return;
    data->prefix_len = len;
}

void markov_set_sentinel_word(MarkovData* data, char* word) {
    if (data->initialized) return;
    data->sentinel_word = strdup(word);
}

void markov_set_output_words(MarkovData* data, int words) {
    if (data->initialized) return;
    data->output_words = words;
}

static int generate_internal(MarkovData* data, int max_words, int (*output_func)(const char*, void*), void* output_data) {
    
    if (! data->initialized) return 1;
    if (max_words == 0) max_words = data->output_words;
    
    const char* prefix[data->prefix_len];
    prepopulate_prefix(data, prefix);
    
    srand( (unsigned int) time(NULL));
    
    while(max_words-- > 0) {
        StateNode* state = lookup_state(data, prefix); //possibly should check for null, but should always exist
        
        const char* word = NULL;
        int count = 0;
        for(SuffixNode* s = state->list; s != NULL; s = s->next) {
            if (rand() % ++count == 0) {
                word = s->suffix;
            }
        }
        
        if (strcmp(word, data->sentinel_word) == 0) {
            break;
        }
        
        if (output_func(word, output_data)) return 1;
                
        rotate_prefix(data, prefix, word);
    }
    
    return 0;
}

static int write_word_to_stream(const char* word, void* data) {
    FILE* dest = (FILE*) data;
    fprintf(dest, "%s ", word);
    
    return 0;
}

typedef struct TextBuffer {
    int size;
    int used;
    char* text;
} TextBuffer;

static int ensure_buffer_size(TextBuffer* buffer, int word_len){
    if (word_len + 1 > (buffer->size - buffer->used)) {
        char* new_buffer = realloc(buffer->text, sizeof(char[buffer->size * 2]));
        if (new_buffer == NULL) {
            return 1;
        }
        buffer->text = new_buffer;
        buffer->size *= 2;
    }
    return 0;
}

static int write_word_to_string(const char* word, void* data){
    TextBuffer* buffer = (TextBuffer*) data;
    
    int word_len = (int)strlen(word);
    if (ensure_buffer_size(buffer, word_len)) return 1;
    
    strcpy(buffer->text + buffer->used, word);
    buffer->used += word_len;
    buffer->text[buffer->used++] = ' '; //replaces \0 with space
    return 0;
}

int markov_generate_to_stream(MarkovData* data, FILE* dest, int max_words) {

    return generate_internal(data, max_words, write_word_to_stream, dest);
}

static TextBuffer* init_textbuffer(int initial_size) {
    TextBuffer* buffer = malloc(sizeof(TextBuffer));
    
    buffer->size = initial_size;
    buffer->used = 0;
    buffer->text = malloc(sizeof(char[initial_size]));
    
    return buffer;
}

char* markov_generate_to_string(MarkovData* data, int max_words) {
    
    const int initial_size = 1024;
    
    TextBuffer* buffer = init_textbuffer(initial_size);
    
    int err = generate_internal(data, max_words, write_word_to_string, buffer);
    
    if (err) {
        free(buffer->text);
        free(buffer);
        return NULL;
    }
    
    buffer->text[buffer->used - 1] = '\0';
    buffer->text = realloc(buffer->text, sizeof(char[buffer->used]));
    char* output = buffer->text;
    free(buffer);
    return output;
}

void markov_free(MarkovData* data) {
    for (int tab_entry = 0; tab_entry < data->statetab_len; tab_entry++) {        
        for(StateNode* node = data->statetab[tab_entry]; node != NULL;){
            
            SuffixNode* suf = node->list;
            while(suf != NULL) {
                free((char *)suf->suffix);
                SuffixNode* next = suf->next;
                free(suf);
                suf = next;
            }
            
            StateNode* next = node->next;
            free(node->prefix);
            free(node);
            node = next;
        }
    }
    
    free((char*)data->sentinel_word);
    free(data->statetab);
    free(data);
}
