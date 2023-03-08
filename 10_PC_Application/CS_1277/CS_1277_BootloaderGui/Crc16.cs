
namespace CS_1277_BootloaderCommon.Utils
{
    public static class Crc16
    {
        private const ushort polynomial = 0xA001;
        private static readonly ushort[] table = new ushort[256];


        static Crc16()
        {
            ushort value;
            ushort temp;

            for (ushort i = 0; i < table.Length; ++i)
            {
                value = 0;
                temp = i;

                for (byte j = 0; j < 8; ++j)
                {
                    if (((value ^ temp) & 0x0001) != 0)
                    {
                        value = (ushort)((value >> 1) ^ polynomial);
                    }
                    else
                    {
                        value >>= 1;
                    }

                    temp >>= 1;
                }

                table[i] = value;
            }
        }


        public static ushort CountCrc(byte[] buff)
        {
            return CountCrc(buff, 0, buff.Length);
        }


        public static ushort CountCrc(byte[] buff, int startIndex, int count)
        {
            ushort crc = 0;

            for (int i = startIndex; i < startIndex + count; ++i)
            {
                byte index = (byte)(crc ^ buff[i]);
                crc = (ushort)((crc >> 8) ^ table[index]);
            }

            return crc;
        }
    }
}
