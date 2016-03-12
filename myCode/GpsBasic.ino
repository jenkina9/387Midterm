// this sketch will allow you to bypass the Atmega chip
// and connect the Ultimate GPS directly to the USB/Serial
// chip converter.
 
// Connect VIN to +5V
// Connect GND to Ground
// Connect GPS RX (data into GPS) to Digital 0
// Connect GPS TX (data out from GPS) to Digital 1
String monthDB[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char input = ' ';
void setup() {
  Serial1.begin(9600);
  Serial1.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
  Serial1.println("$PMTK220,200*2C");
  Serial1.println("$PMTK300,200,0,0,0,0*2F");

  Serial.begin(9600);
}
void loop() {
  String thing,gmt,lat,ns,lon,ew,knots,compass,day,month,year = "";
  boolean fix = false;
  //input = Serial.read();
  //while(input != '!'){
   // input = Serial.read();
  //}
    if(Serial1.read() == '$'){
      thing = Serial1.readStringUntil('$');
      //Serial.print('$' + thing);
     //thing = Serial1.readStringUntil('*');
     //Serial.print('$' + thing + '*');
     //Serial.print(Serial1.read());
     //Serial.println(Serial1.read());
    }
    if(thing[1] == 'P') {
      char current[thing.length()];
      thing.toCharArray(current,thing.length());
      int i = 0;
      
      //skip the letters
      while(current[i] != ','){
        i++;
      }
      i++;

      //get Time
      gmt = current[i];
      i++;
      gmt += current[i];
      gmt += ':';
      i++;
      gmt += current[i];
      i++;
      gmt += current[i];
      while(current[i] != ','){
       i++;
      }
      i++;

      //Check Fixed or not
      if(current[i] == 'A'){
        fix = true;
      }
      i += 2;

      //get Latitude
      while(current[i] != ','){
        lat += current[i];
        i++;
      }
      i++;

      //get North/South
      if(current[i] == 'N'){
        ns = "North";
      } else if(current[i] == 'S'){
        ns = "South";
      }
      i += 2;

      //get Longitude
      while(current[i] != ','){
        lon += current[i];
        i++;
      }
      i++;

      //get East/West
      if(current[i] == 'E'){
        ns = "East";
      } else if(current[i] == 'W'){
        ns = "West";
      }
      i += 2;

      //get knots
      while(current[i] != ','){
        knots += current[i];
        i++;
      }
      i++;

      //get compass direction
      while(current[i] != ','){
        compass += current[i];
        i++;
      }
      i++;

      day += current[i];
      i++;
      day += current[i];
      i++;

      month += current[i];
      i++;
      month += current[i];
      i++;

      month = monthDB[month.toInt() - 1];

      year += current[i];
      i++;
      year += current[i];
      i++;

      if(Serial.read() == '!'){
        Serial.println("Time: " + gmt + " GMT");
        Serial.println(month + " " + day + ',' + " " + year);
        if(fix){
          Serial.println(lat + ns);
         Serial.println(lon + ew);
         Serial.println("Speed: " + knots + " Knots");
         Serial.println("Direction " + compass + " Format when I figure out how");
        } else {
          Serial.println("Could not find fix with Satelite");
        }
      }
      input = ' ';
     
    }
  }
