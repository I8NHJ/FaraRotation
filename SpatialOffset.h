//      moon2(Year, Month, Day, UTCTime, MyLong, MyLat, &MoonRAscension, &MoonDeclination, &TopRAscension, &TopDeclination, &LST, &HA, &MoonAz, &MoonEl, &MoonDist);

  void PolarAxisOffset (double MLat, double MAz, double MEl, double TLat, double TAz, double TEl, int *OffsetAngle) {
    double rad = 57.2957795131;
    double Temp1;
    double Temp2;

    Temp1 = sin(MLat/rad) * cos(MEl/rad) - cos(MLat/rad) * cos(MAz/rad) * sin(MEl/rad);
    Temp2 = cos(MLat/rad) * sin (MAz/rad);
    double MPoloOffset = rad*atan2(Temp1, Temp2);

    Temp1 = sin(TLat/rad) * cos(TEl/rad) - cos(TLat/rad) * cos(TAz/rad) * sin(TEl/rad);
    Temp2 = cos(TLat/rad) * sin (TAz/rad);
    double TPoloOffset = rad*atan2(Temp1, Temp2);

    *OffsetAngle=round(fmod (MPoloOffset-TPoloOffset+720.0, 180.0));

    if(*OffsetAngle > 90) {*OffsetAngle=*OffsetAngle-180;}
  }

  void FaradayOffset (int RXAngle, int OffsetAngle, int *TXAngle) {
    *TXAngle = OffsetAngle + (OffsetAngle - RXAngle);
  }