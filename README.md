MCu menggunakan ESP01
Saklar menggunakan NORMALY CLOSED jadi logikanya dibalik, kalo HIGH saklar open (Mati) dan LOW closed (Hidup)


Jika koneksi ke server telegram bot gagal ganti fingerprint pada file CTBot/src/CTBotSecureConnection.h dengan dibawah :

uint8_t m_fingerprint[20]{ 0x8A, 0x10, 0xB5, 0xB9, 0xB1, 0x57, 0xAB, 0xDA, 0x19, 0x74, 0x5B, 0xAB, 0x62, 0x1F, 0x38, 0x03, 0x72, 0xFE, 0x8E, 0x47 }; // use this preconfigured fingerprrint by default (2023/04/27)