/**
 * @file serial.c
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of serialization functions
 * @date 2023-08-08
 *
 */

#include "serial.h"


/**
 * @brief Serialize a G1 element : g1_t element
 *
 * @param g1  the element to serialize
 * @param buf buffer conatins serialization of g1; it must be allocated with the
 *            right size : G1_SIZE_BIN + sizeof(uint8_t)
 * @return size of the serialized element in bytes or 0 if errors occured
 */
int serialize_g1_element(const g1_t g1, uint8_t buf[]) {

	if (buf == NULL) return 0;

	uint8_t size = g1_size_bin(g1, 0);
	memcpy(buf, &size, sizeof(uint8_t));
	g1_write_bin(buf + sizeof(uint8_t), size, g1, 0);
	
	return (sizeof(uint8_t) + size);
}

/**
 * @brief Deserialize a G1 element : g1_t element
 * 
 * @param buf the buffer to deserialize in g1
 * @param g1 the result of the deserialization of buf, must be allocated before
 *           calling the function
 * @return size of the serialized element in bytes or 0 if errors occured
 */
int deserialize_g1_element(const uint8_t buf[], g1_t g1) {
	
	if (buf == NULL) return 0;

	uint8_t size = 0;
	memcpy(&size, buf, sizeof(uint8_t));
	g1_read_bin(g1, buf + sizeof(uint8_t), size);

	return (sizeof(uint8_t) + size);
}

}

/**
 * @brief Serialize a vector of G1 elements : G1_VS_VECT element
 * 
 * @param vect  the vector to serialize
 * @param buf_serialized buffer to store the serialized vector; *buf is allocated inside the function
 * @param size size of the serialized vector in bytes
 * @return 0 if success, an error code otherwise
 */
int serialize_g1_vector(const G1_VS_VECT vect, uint8_t** buf, int* size) {

	int shift = 0;
	int g1_size = 0;
	int total_size = 0;
	uint8_t *buf_tmp = NULL;

  if (vect && vect->coord) {

		// Calculate total size needed for serialization
		total_size = vect->dim;
		for (uint8_t i = 0; i < vect->dim; i++)
			total_size += g1_size_bin(vect->coord[i], 0);

    if ((buf_tmp = (uint8_t*) malloc(total_size)) == NULL) {
      fprintf(stderr, "Error in serialize_g1_vector: malloc failed\n");
			return 1;
    }

    for (uint8_t i = 0; i < vect->dim; i++) {
			if ((g1_size = serialize_g1_element(vect->coord[i], buf_tmp + shift)) == 0) {
				fprintf(stderr, "Error in serialize_g1_vector: serialize_g1_element failed\n");
				free(buf_tmp);
				return 2;
			}
      shift += g1_size;
    }

		// At this point, shift is the size of the buffer representing the g1 vector
		// shift must be egal to total_size
		if (shift != total_size) {
			fprintf(stderr, "Error in serialize_g1_vector: shift != total_size\n");
			free(buf_tmp);
			return 3;
		}

    total_size += sizeof(int) + sizeof(uint8_t);
    if ((*buf = (uint8_t*) malloc(total_size)) == NULL) {
      fprintf(stderr, "Error in serialize_g1_vector: malloc failed\n");
      free(buf_tmp);
			return 4;
    }

		// write size of buffer representing the g1 vector, dim of g1 vector and g1 vector
    memcpy(*buf, &shift, sizeof(int));
    memcpy(*buf + sizeof(int), &vect->dim, sizeof(uint8_t));
    memcpy(*buf + sizeof(int) + sizeof(uint8_t), buf_tmp, shift);
		*size = total_size;

    free(buf_tmp);
  }
  else {
    fprintf(stderr, "Error in serialize_g1_vector: the g1 vector is NULL\n");
		return 5;
  }

  return 0;
}

/**
 * @brief Deserialize a vector of G1 elements : G1_VS_VECT element
 * 
 * @param buf buffer to deserialize
 * @param size buffer size
 * @param dest_vect result of the deserialization of buf; *dest_vect is allocated inside the function
 * @return 0 if success, an error code otherwise
 */
int deserialize_g1_vector(const uint8_t* buf, const int size, g1_vect_t** dest_vect) {
	int ret = 0;
  int buf_size = 0;
	int g1_size = 0;
  int shift = 0;
  uint8_t dim = 0;

  if (buf != NULL && memcpy(&buf_size, buf, sizeof(int)) && buf_size > 0) {
    memcpy(&dim, buf + sizeof(int), sizeof(uint8_t));
    shift = sizeof(int) + sizeof(uint8_t);

    *dest_vect = dpvs_create_g1_vect(dim);
    G1_VS_VECT vect = *dest_vect;
    if (vect != NULL) {
      for (uint8_t i = 0; i < dim; i++) {
        g1_size = deserialize_g1_element(buf + shift, vect->coord[i]);
        if (g1_size == 0) {
					fprintf(stderr, "Error deserializing the g1 vector: deserialize_g1_element failed.\n");
					dpvs_clear_g1_vect(vect);
					ret++;
					break;
				}				
				shift += g1_size;
      }

      if (shift != size || (shift - sizeof(int) - sizeof(uint8_t)) != buf_size) {
        fprintf(stderr, "Error deserializing the g1 vector: the size of the buffer is not correct.\n");
        dpvs_clear_g1_vect(vect);
				ret++;
      }
    }
    else {
      fprintf(stderr, "Error deserializing the g1 vector: dpvs_create_g1_vect failed.\n");
			ret++;
    }
  }
  else {
    fprintf(stderr, "Error deserializing the g1 vector: the buffer is NULL or the size is not correct.\n");
    ret++;
  }

  return ret;
}
