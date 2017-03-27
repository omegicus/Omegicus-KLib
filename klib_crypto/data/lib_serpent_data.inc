use32
align 4
SERPENT_data:
      SERPENT_l_key   rd 140; 140 DUP 0
      SERPENT_m_key   rd 280; 280 DUP 0
      SERPENT_block   rb 32;32 dup 0
SERPENT_data_sz = 140*4 + 280*4 + 32;$ - SERPENT_data