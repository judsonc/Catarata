#include "process.h"

#define DEBUG 1

Img *greyscale(Img *img)
{
  ushort toned;
  for (ushort i = 0; i < img->height; ++i)
  {
    for (ushort j = 0; j < img->width; ++j)
    {
      toned = (img->pixels[i][j].r * 0.3 + img->pixels[i][j].g * 0.59 + img->pixels[i][j].b * 0.11);
      img->pixels[i][j].r = toned;
      img->pixels[i][j].g = toned;
      img->pixels[i][j].b = toned;
    }
  }

  printf("The image was toned to its greyscale.\n");

  return img;
}

Img *gaussianFilter(Img *originalImg, uchar limit)
{
  if (limit <= 0)
  {
    fprintf(stderr, "Can't execute %i times the Gaussian filter, please give a valid number (greater than 0).\n", limit);
    return NULL;
  }

  Img *gaussImg = createImg(originalImg->height, originalImg->width, originalImg->max_rgb);

  uchar gauss[7][7] = {{1, 1, 2, 2, 2, 1, 1}, {1, 3, 4, 5, 4, 3, 1}, {2, 4, 7, 8, 7, 4, 2}, {2, 5, 8, 10, 8, 5, 2}, {2, 4, 7, 8, 7, 4, 2}, {1, 3, 4, 5, 4, 3, 1}, {1, 1, 2, 2, 2, 1, 1}};
  float filter;
  float normalizationFactor = 170;
  static uchar count = 0;

  // uncomment these two lines if you want to use a 5x5 Gaussian kernel
  // int gauss[5][5] = {{2, 4, 5, 4, 2}, {4, 9, 12, 9, 4}, {5, 12, 12, 12, 5}, {4, 9, 12, 9, 4}, {2, 4, 5, 4, 2}};
  // double normalizationFactor = 159;

  for (ushort i = 0; i < gaussImg->height; i++)
  {
    for (ushort j = 0; j < gaussImg->width; j++)
    {
      // this will get each pixel of img->**pixels
      // each pixel will be the "15" on the matrix (gauss[2][2])

      filter = 0;
      for (uchar k = 0; k < 7; ++k)
      {
        for (uchar l = 0; l < 7; ++l)
        {
          if (((i - 3 + k) >= 0 && (j - 3 + l) >= 0) && (i + 3 < gaussImg->height && j + 3 < gaussImg->width))
          {
            filter += originalImg->pixels[i - 3 + k][j - 3 + l].r * gauss[k][l] / normalizationFactor;
            // printf("filter executed\n");
          }
        }
      }

      gaussImg->pixels[i][j].r = filter;
      gaussImg->pixels[i][j].g = filter;
      gaussImg->pixels[i][j].b = filter;
    }
  }

  if (!gaussImg)
  {
    fprintf(stderr, "Error while blurring. Resulting image is NULL.\n");
    freeImg(gaussImg);
    freeImg(originalImg);
  }
  else
  {
    if (count == 0)
    {
      printf("The image was succesfully blurred %i time.\n", count + 1);
      ++count;
    }
    else
    {
      printf("The image was succesfully blurred %i times.\n", count + 1);
      ++count;
    }
  }

  if (limit > 1)
  {
    gaussImg = gaussianFilter(gaussImg, limit - 1);
  }
  else if (limit == 1)
  {
    freeImg(originalImg);
    return gaussImg;
  }

  return gaussImg;
}

Img *sobelFilter(Img *originalImg, uchar limit)
{
  if (limit <= 0)
  {
    fprintf(stderr, "Can't execute %i times the Sobel filter, please give a valid number (greater than 0).\n", limit);
    return NULL;
  }

  Img *sobelImg = createImg(originalImg->height, originalImg->width, originalImg->max_rgb);

  int sobel_x[3][3] = {{3, 0, -3}, {10, 0, -10}, {3, 0, -3}};
  int sobel_y[3][3] = {{3, 10, 3}, {0, 0, 0}, {-3, -10, -3}};
  // int sobel_x[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
  // int sobel_y[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
  float filter_x;
  float filter_y;
  static uchar count = 0;

  for (ushort i = 0; i < sobelImg->height; ++i)
  {
    for (ushort j = 0; j < sobelImg->width; ++j)
    {
      /* this will get each pixel of img->**pixels; each pixel will be on the "0"
			 * on either matrix (sobel_x[1][1] and sobel_y[1][1] */

      // sobel_x and sobel_y covolution
      filter_x = 0;
      filter_y = 0;
      for (uchar k = 0; k < 3; ++k)
      {
        for (uchar l = 0; l < 3; ++l)
        {
          // this tests if the pixel is on a border
          if ((i == 0 || j == 0) || (i == sobelImg->height - 1 || j == sobelImg->width - 1))
          {
            // if it is, then its value is 0
            filter_x += 0;
            filter_y += 0;
          }
          else
          {
            // else, we multiply
            filter_x += originalImg->pixels[i - 1 + k][j - 1 + l].r * sobel_x[k][l];
            filter_y += originalImg->pixels[i - 1 + k][j - 1 + l].r * sobel_y[k][l];
          }
        }
      }

      float gradient = sqrt(filter_x * filter_x + filter_y * filter_y);

      // here we test if the magnitude is greater then the MAX_RGB value
      // TODO replace MAX_RGB with originalImg->max_rgb
      if (gradient > MAX_RGB)
      {
        sobelImg->pixels[i][j].r = originalImg->max_rgb;
        sobelImg->pixels[i][j].g = originalImg->max_rgb;
        sobelImg->pixels[i][j].b = originalImg->max_rgb;
      }
      else
      {
        sobelImg->pixels[i][j].r = (uchar)gradient;
        sobelImg->pixels[i][j].g = (uchar)gradient;
        sobelImg->pixels[i][j].b = (uchar)gradient;
      }
    }
  }

  if (!sobelImg)
  {
    fprintf(stderr, "Error while aplying Sobel's Filter. Resulting image is NULL.\n");
    freeImg(sobelImg);
    freeImg(originalImg);
  }
  else
  {
    if (count == 0)
    {
      printf("Sobel's filter was successfully applied %i time.\n", count + 1);
      ++count;
    }
    else
    {
      printf("Sobel's filter was successfully applied %i times.\n", count + 1);
      ++count;
    }
    // printf("Sobel's filter was successfully applied.\n");
  }

  if (limit > 1)
  {
    sobelImg = sobelFilter(sobelImg, limit - 1);
  }
  else if (limit == 1)
  {
    freeImg(originalImg);
    return sobelImg;
  }

  return sobelImg;
}

Img *threshold(Img *originalImg, uchar intensity)
{
  Img *thresholdImg = createImg(originalImg->height, originalImg->width, originalImg->max_rgb);
  int pixelValue = 0;
  for (ushort i = 0; i < thresholdImg->height; i++)
  {
    for (ushort j = 0; j < thresholdImg->width; j++)
    {
      // for each pixel, we will check if is bigger than X, if it is: set to max_rgb
      if (originalImg->pixels[i][j].r > intensity)
      {
        pixelValue = thresholdImg->max_rgb;
        // printf("Pixel [%i][%i](%i) is bigger than %i.\n", i, j, originalImg->pixels[i][j].r, intensity);
      }
      else
      {
        pixelValue = 0;
        // printf("Pixel [%i][%i](%i) is smaller than %i.\n", i, j, thresholdImg->pixels[i][j].r, intensity);
      }

      thresholdImg->pixels[i][j].r = pixelValue;
      thresholdImg->pixels[i][j].g = pixelValue;
      thresholdImg->pixels[i][j].b = pixelValue;
    }
  }

  printf("Threshold was successfully applied with intensity %hhu.\n", intensity);
  freeImg(originalImg);
  return thresholdImg;
}

Img *otsuMethod(Img *originalImg)
{
  Img *otsuImg = createImg(originalImg->height, originalImg->width, originalImg->max_rgb);
  ushort max_val = originalImg->max_rgb + 1;
  int total_pixel = originalImg->height * originalImg->width;
  uchar threshold;
  ushort *histogram = (ushort *)calloc(max_val, sizeof(ushort));

  // Calculate the images histogram.
  // Since we will build the array based on the pixels, we don't need the max rgb
  uchar pixel;
  for (ushort i = 0; i < originalImg->height; ++i)
  {
    for (ushort j = 0; j < originalImg->width; ++j)
    {
      pixel = originalImg->pixels[i][j].r;
      ++histogram[pixel];
    }
  }

  /**
	 * Here we are using a bit faster approach to Otsu's method. With some
	 * manipulation we can calcualte the threshold by calculating the 'between
	 * class' variance. The threshold with the maximum 'between class' variance 
	 * also has the minimum 'within class' variance, which allows us to calculate
	 * the best threshold.
	 */

  // sum to calculate the foreground mean (total sum minus background sum)
  int sum = 0;
  for (int i = 0; i < max_val; ++i)
  {
    sum += i * histogram[i];
  }

  float sumBack = 0;
  float weightBack = 0;
  float weightFore = 0;

  float varMax = 0;
  threshold = 0;

  // here's where the magic happens
  for (int i = 0; i < max_val; ++i)
  {
    // background weight
    weightBack += histogram[i];
    if (weightBack == 0)
      continue;

    // foreground weight
    weightFore = total_pixel - weightBack;
    if (weightFore == 0)
      break;

    // background sum
    sumBack += (float)(i * histogram[i]);

    // mean background and foreground
    float meanBack = sumBack / weightBack;
    float meanFore = (sum - sumBack) / weightFore;

    // this is the 'between class' calculation
    float varBetween = (float)weightBack * (float)weightFore * (meanBack - meanFore) * (meanBack - meanFore);

    // check if we have a new max value
    if (varBetween > varMax)
    {
      varMax = varBetween;
      threshold = i;
    }
  }

  // here we set the return Img's pixels to be 0 or 1 (255)
  uchar pixelValue;
  for (ushort i = 0; i < originalImg->height; ++i)
  {
    for (ushort j = 0; j < originalImg->width; ++j)
    {
      if (originalImg->pixels[i][j].r > threshold)
      {
        pixelValue = originalImg->max_rgb;
      }
      else
      {
        pixelValue = 0;
      }

      otsuImg->pixels[i][j].r = pixelValue;
      otsuImg->pixels[i][j].g = pixelValue;
      otsuImg->pixels[i][j].b = pixelValue;
    }
  }

  free(histogram);
  free(originalImg);
  return otsuImg;
}

Circles *houghMethod(ImgBin *img)
{
  // Declaring circle that we will store the biggest circles
  Circles *circles = (Circles *)calloc(1, sizeof(Circles));
  circles->iris = (Iris *)calloc(15, sizeof(Iris));

  // Setting up some env variables
  int rmax = (img->width + img->height - bigger(img->width, img->height)) / 4;
  int rmin_iris = (img->width + img->height - bigger(img->width, img->height)) / 8;
  int rmin = (img->width + img->height - bigger(img->width, img->height)) / 17;

  int a, b;

  // printf("DEBUG MESSAGE:\trmin: %i;\n", rmin);
  // printf("DEBUG MESSAGE:\trmin_iris: %i\n", rmin_iris);
  // printf("DEBUG MESSAGE:\trmax: %i;\n", rmax);

  // Declaration of the Circles Matrix (a.k.a. accumulator)
  Acc *acc = createAcc(img->height, img->width, rmax - rmin + 1);

  // Making some calculations to reduce program's runtime
  double *sin = preCalcSin();
  double *cos = preCalcCos();

  for (int y = 0; y < img->height; y++)
  {

    if (y % 100 == 0)
      printf("Processing line %i of %i...\n", y, img->height);

    for (int x = 0; x < img->width; x++)
    {
      if (img->pixels[y][x] == 0)
      { // That means: If the pixel(y, x) is an edge...
        for (int r = rmin; r <= rmax; r += 5)
        {
          for (int theta = 0; theta < 360; theta += 15)
          {
            a = x + r * cos[theta];
            b = y - r * sin[theta];
            /* if 'a' and 'b' are into the limits
						(bigger than 0 and smaller than height/width) */
            if ((a >= 0 && b >= 0) && (a < img->width && b < img->height))
            {
              acc->accumulator[b][a][r - rmin]++;
            }
          }
        }
      }
    }
  }
  // Discover the biggest value on accumulator
  int max = 0;
  int turn = 0;

  for (int y = rmax; y < img->height; ++y)
  {
    for (int x = rmax; x < img->width; ++x)
    {
      for (int r = rmin_iris; r <= rmax; r += 2)
      {
        if (max != bigger(max, acc->accumulator[y][x][r - rmin]))
        {
          circles->iris[turn].x = x, circles->iris[turn].y = y, circles->iris[turn].rad = r;
        }
        max = bigger(max, acc->accumulator[y][x][r - rmin]);
      }
    }
  }
  circles->iris[turn].area = pow(circles->iris[turn].rad, 2) * 3.14159;
  printf("Circle area: %.0lf\n", circles->iris[turn].area);
  return (circles);
}

int count(Img *img)
{
  int times = 0;
  for (int y = 0; y < img->height; y++)
  {
    for (int x = 0; x < img->width; x++)
    {
      if (img->pixels[y][x].r == 255)
      {
        // printf("pixel claro\n");
        times++;
      }
    }
  }
  // printf("times on count(): %lf\n", times);
  return (times);
}