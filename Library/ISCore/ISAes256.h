#pragma once
#ifndef _ISAES256_H_INCLUDED
#define _ISAES256_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
#define BLOCK_SIZE 16
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISAes256
{
public:
	ISAes256(const std::vector<unsigned char> &key);
	~ISAes256();

	static size_t encrypt(const std::vector<unsigned char> &key, const std::vector<unsigned char> &plain, std::vector<unsigned char> &encrypted);
	static size_t encrypt(const std::vector<unsigned char> &key, const unsigned char *plain, const size_t plain_length, std::vector<unsigned char> &encrypted);
	static size_t decrypt(const std::vector<unsigned char> &key, const std::vector<unsigned char> &encrypted, std::vector<unsigned char> &plain);
	static size_t decrypt(const std::vector<unsigned char> &key, const unsigned char *encrypted, const size_t encrypted_length, std::vector<unsigned char> &plain);

	size_t encrypt_start(const size_t plain_length, std::vector<unsigned char> &encrypted);
	size_t encrypt_continue(const std::vector<unsigned char> &plain, std::vector<unsigned char> &encrypted);
	size_t encrypt_continue(const unsigned char *plain, const size_t plain_length, std::vector<unsigned char> &encrypted);
	size_t encrypt_end(std::vector<unsigned char> &encrypted);

	size_t decrypt_start(const size_t encrypted_length);
	size_t decrypt_continue(const std::vector<unsigned char> &encrypted, std::vector<unsigned char> &plain);
	size_t decrypt_continue(const unsigned char *encrypted, const size_t encrypted_length, std::vector<unsigned char> &plain);
	size_t decrypt_end(std::vector<unsigned char> &plain);

private:
	void check_and_encrypt_buffer(std::vector<unsigned char> &encrypted);
	void check_and_decrypt_buffer(std::vector<unsigned char> &plain);

	void encrypt(unsigned char *buffer);
	void decrypt(unsigned char *buffer);

	void expand_enc_key(unsigned char *rc);
	void expand_dec_key(unsigned char *rc);

	void sub_bytes(unsigned char *buffer);
	void sub_bytes_inv(unsigned char *buffer);

	void copy_key();

	void add_round_key(unsigned char *buffer, const unsigned char round);

	void shift_rows(unsigned char *buffer);
	void shift_rows_inv(unsigned char *buffer);

	void mix_columns(unsigned char *buffer);
	void mix_columns_inv(unsigned char *buffer);

private:
	std::vector<unsigned char> m_key;
	std::vector<unsigned char> m_salt;
	std::vector<unsigned char> m_rkey;

	unsigned char m_buffer[3 * BLOCK_SIZE];
	unsigned char m_buffer_pos;
	size_t m_remainingLength;

	bool m_decryptInitialized;
};
//-----------------------------------------------------------------------------
#endif
