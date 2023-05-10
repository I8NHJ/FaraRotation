//------------------------------------------------------
void maidenhead_to_coordinates(char* grid, float* latitude_degrees,float* longitude_degrees){

  /*  Base code contributed by Adam Maurer VK4GHZ 14-JUL-2020

      Input: grid, four or six characters (i.e. FN20, FN20EV, fn20, fn20ev, Fn20eV)
      Output: latitude_degrees, longitude_degrees

  */

  float alpha1 = 0;
  float alpha2 = 0;
  byte alpha3 = 0;
  byte alpha4 = 0;
  float alpha5 = 0;
  float alpha6 = 0;

  const int x_step = 20; // Number of Horizontal degrees EW across a major field
  const int y_step = 10; // Number of Vertical degrees NS across a major field

  // Uppercase everything
  for (int x = 0;x < 6;x++){
    grid[x] = toupper(grid[x]);
  }

  // Do we have a subsquare?  If not, fudge it in the middle of the square
  if (!((grid[4] > 64) && (grid[4] < 89) && (grid[5] > 64) && (grid[5] < 89))){
    grid[4] = 77;
    grid[5] = 77;
  }

  // Convert Alphas to numeric values to calculate with
  // A=65... R=82... X=88

  alpha1 = (grid[0]-65) * x_step;
  alpha2 = (grid[1]-65) * y_step;
  alpha3 = (grid[2]-48) * (x_step/10);
  alpha4 = (grid[3]-48) * (y_step/10);

  // Each Field is subdivided into 24 x 24 sub squares
  alpha5 = ((grid[4]-65) * (x_step/240.0)) + (x_step/480.0); 
  alpha6 = ((grid[5]-65) * (y_step/240.0)) + (y_step/480.0);

  *longitude_degrees = alpha1 + alpha3 + alpha5 - 180.0;

  *latitude_degrees = (alpha2 + alpha4 + alpha6) - 90.0;

}

char *coordinates_to_maidenhead(float latitude_degrees,float longitude_degrees){

  static char temp_string[8] = "";

  latitude_degrees += 90.0;
  longitude_degrees += 180.0;

  temp_string[0] = (int(longitude_degrees/20)) + 65;
  temp_string[1] = (int(latitude_degrees/10)) + 65;
  temp_string[2] = (int((longitude_degrees - int(longitude_degrees/20)*20)/2)) + 48;
  temp_string[3] = (int(latitude_degrees - int(latitude_degrees/10)*10)) + 48;
  temp_string[4] = (int((longitude_degrees - (int(longitude_degrees/2)*2)) / (5.0/60.0))) + 97;
  temp_string[5] = (int((latitude_degrees - (int(latitude_degrees/1)*1)) / (2.5/60.0))) + 97;
  temp_string[6] = 0;

  return temp_string;

}
