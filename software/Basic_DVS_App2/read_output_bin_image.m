fp = fopen('output.bin', 'rb');

fractional_bits = 5;

width = fread(fp, 1, 'uint32');
height = fread(fp, 1, 'uint32');

J = reshape(fread(fp, width*height, 'int16'), [width height])'/(2^fractional_bits);

figure; imshow(uint8(J));

fclose(fp);