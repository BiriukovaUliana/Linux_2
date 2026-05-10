#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define ALIGN 8                      
#define ALIGN_UP(n) (((n) + ALIGN-1) & ~(ALIGN-1))
#define MIN_BLOCK 32      

typedef struct block_header {
    size_t size;                     
    int    free;                        
    struct block_header *next;        
    struct block_header *prev;     
} block_header_t;

#define HEADER_SIZE (sizeof(block_header_t))

static block_header_t *free_list = NULL; 

static block_header_t *request_block(size_t size) {
    block_header_t *block = sbrk(0);      
    void *req = sbrk(HEADER_SIZE + size);  
    if (req == (void*)-1) return NULL;  

    block->size = size;
    block->free = 0;
    block->next = NULL;
    block->prev = NULL;
    return block;
}

static void coalesce(block_header_t *block) {
    if (block->next && block->next->free) {
        block->size += HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
    if (block->prev && block->prev->free) {
        block->prev->size += HEADER_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

void *my_malloc(size_t size) {
    if (size == 0) return NULL;

    size = ALIGN_UP(size);    
    if (size < MIN_BLOCK) size = MIN_BLOCK;

    block_header_t *cur = free_list;
    while (cur) {
        if (cur->free && cur->size >= size) {

            if (cur->size >= size + HEADER_SIZE + MIN_BLOCK) {
                block_header_t *new_block =
                    (block_header_t*)((char*)cur + HEADER_SIZE + size);
                new_block->size = cur->size - size - HEADER_SIZE;
                new_block->free = 1;
                new_block->next = cur->next;
                new_block->prev = cur;
                if (cur->next) cur->next->prev = new_block;
                cur->next = new_block;
                cur->size = size;
            }
            cur->free = 0;
            return (char*)cur + HEADER_SIZE; 
        }
        cur = cur->next;
    }

    block_header_t *block = request_block(size);
    if (!block) return NULL;

    if (!free_list) {
        free_list = block;
    } else {
        block_header_t *tail = free_list;
        while (tail->next) tail = tail->next;
        tail->next = block;
        block->prev = tail;
    }
    return (char*)block + HEADER_SIZE;
}

void my_free(void *ptr) {
    if (!ptr) return;

    block_header_t *block = (block_header_t*)((char*)ptr - HEADER_SIZE);
    if (block->free) {
        fprintf(stderr, "my_free: подвійне звільнення!\n");
        return;
    }
    block->free = 1;
    coalesce(block);    
}
void *my_calloc(size_t nmemb, size_t size) {
    void *ptr = my_malloc(nmemb * size);
    if (ptr) memset(ptr, 0, nmemb * size);
    return ptr;
}

void *my_realloc(void *ptr, size_t size) {
    if (!ptr) return my_malloc(size);
    if (size == 0) { my_free(ptr); return NULL; }

    block_header_t *block = (block_header_t*)((char*)ptr - HEADER_SIZE);
    if (block->size >= size) return ptr;

    void *new_ptr = my_malloc(size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, block->size);
    my_free(ptr);
    return new_ptr;
}

void my_heap_dump(void) {
    printf("\nHEAP DUMP\n");
    printf("%-6s %-18s %-10s %-8s\n", "№", "Адреса", "Розмір", "Стан");
    printf("%-6s %-18s %-10s %-8s\n", "---", "------------------", "--------", "------");
    block_header_t *cur = free_list;
    int i = 0;
    size_t total = 0, free_bytes = 0;
    while (cur) {
        printf("%-6d %-18p %-10zu %-8s\n",
               i++,
               (void*)((char*)cur + HEADER_SIZE),
               cur->size,
               cur->free ? "ВІЛЬНИЙ" : "ЗАЙНЯТИЙ");
        total += cur->size;
        if (cur->free) free_bytes += cur->size;
        cur = cur->next;
    }
    printf("Всього: %zu байт, вільно: %zu байт\n\n", total, free_bytes);
}

int main(void) {
    printf("Тест 1: базове виділення\n");
    int *a = my_malloc(sizeof(int) * 5);
    char *b = my_malloc(100);
    double *c = my_malloc(sizeof(double));
    a[0] = 42; a[4] = 99;
    strcpy(b, "Привіт, менеджер пам'яті!");
    *c = 3.14;
    printf("a[0]=%d, a[4]=%d\n", a[0], a[4]);
    printf("b=\"%s\"\n", b);
    printf("c=%.2f\n", *c);
    my_heap_dump();

    printf("Тест 2: звільнення і злиття блоків\n");
    my_free(b);
    my_free(a);
    my_heap_dump(); 

    printf("Тест 3: повторне використання вільного блоку\n");
    int *d = my_malloc(sizeof(int) * 3);
    d[0] = 7; d[1] = 8; d[2] = 9;
    printf("d = {%d, %d, %d}\n", d[0], d[1], d[2]);
    my_heap_dump();

    printf("Тест 4: calloc (обнулення\n");
    int *e = my_calloc(5, sizeof(int));
    printf("e = {%d, %d, %d, %d, %d} (мають бути нулі)\n",
           e[0], e[1], e[2], e[3], e[4]);

    printf("Тест 5: realloc\n");
    e = my_realloc(e, sizeof(int) * 10);
    e[9] = 123;
    printf("e[9] після realloc = %d\n", e[9]);
    my_heap_dump();

    printf("Тест 6: подвійне звільнення (захист)\n");
    my_free(e);
    my_free(e); 

    printf("\nВсі тести завершено.\n");
    return 0;
}
