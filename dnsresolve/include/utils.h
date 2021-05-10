/**
 *  This header just defines some useful macros.
 * */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <zephyr.h>

#define CRC_INIT	0xffffffff
#define CRC_POLY	0x04c11db7

#define REQUIRE_MODULE_STATE(state, state_name, req, req_name)                  \
    if ((req) != (state)) {                                                     \
        LOG_ERR("[%s] Invalid operation in state <%s> (required <%s>)",         \
                __func__, state_name, req_name);                                \
        return -EACCES;                                                         \
    }

#define REQUIRE(e, r)                                                           \
    if (!(e)) {                                                                 \
        LOG_ERR("[%s] Required (%s) which was not the case.", __func__, #e);    \
        return r;                                                               \
    }

#define IS_DIGIT(d) ('0' <= (d) && (d) <= '9')

/**
 *  @brief  This structure contains all the item information for a FIFO queue element.
*/
struct fifo_meta {
    void        *fifo_reserved; //< Used by Zephyr's FIFO implementation.
    atomic_t    is_used;        //< Flag indicating whether the item has been processed.
};

/**
 *  @brief  Calculates an MPEG2 CRC for a given buffer.
 * 
 *  @param  data    Data over which the CRC is to be calculated.
 *  @param  len     Length of the data buffer in bytes.
*/
static inline  uint32_t crc_mpeg2(const uint8_t * data, const uint16_t len) {
    uint32_t crc = CRC_INIT;
	for (uint16_t i = 0; i < len; i++) {
		crc ^= (((const uint32_t) data[i]) << 24);
		for (uint16_t j = 0; j < 8; j++) {
            uint32_t msb = crc >> 31;
			crc <<= 1;
			crc ^= (0 - msb) & CRC_POLY;
		}
	}
	return crc;
}

/**
 *  @brief  Checks whether a FIFO item is currently in use.
 * 
 *  @param  item    FIFO item to be checked. The item MUST have a struct fifo_meta as first member.
 * 
 *  @return True if the value is currently in use, false otherwise.
*/
static inline bool fifo_meta_in_use(void *item) {
    return atomic_get(&((struct fifo_meta *) item)->is_used) != 0;
}


/**
 *  @brief  Marks a FIFO item as being used.
 * 
 *  @param  item    FIFO item to be checked. The item MUST have a struct fifo_meta as first member.
 * 
 *  @return True if the operation was successful - false otherwise.
*/
static inline bool fifo_meta_mark_in_use(void *item) {
    return atomic_cas(&((struct fifo_meta *) item)->is_used, 0, 1);
}


/**
 *  @brief  Marks a FIFO item as being free.
 * 
 *  @param  item    FIFO item to be checked. The item MUST have a struct fifo_meta as first member.
*/
static inline void fifo_meta_mark_free(void *item) {
    atomic_clear(&((struct fifo_meta *) item)->is_used);
}

#endif
