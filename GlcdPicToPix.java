import java.awt.image.*;
import javax.imageio.ImageIO;
import java.io.*;

public class GlcdPicToPix {
  
  public static void main (String [] args){
    try {
      PrintWriter out = new PrintWriter (new FileWriter ("out.txt"));
      BufferedImage bufferedImg = ImageIO.read(new File ("glcdpic4.png"));
      String prev = "";
      int jPrev = 0, sameCount = 0, iPrev = 0;
      for (int i = 0; i<128; i++){
        for (int j = 0; j<128; j++){
          if (bufferedImg.getRGB(i, j) != -1){
            int temp = bufferedImg.getRGB(i,j);
            temp = temp & 0xFFFFFF;
            String colHex = Integer.toHexString(temp);
            if (!colHex.equals (prev) || (sameCount == 0 && j != jPrev + 1)){
              if (sameCount > 0 && !colHex.equals (prev))
                out.println ("    glcdDrawRectangle(" + i + ", " + i + ", " + jPrev + ", " + (j-1) + ", 0x" + prev + ");");
              else if (prev != "")
                out.println ("    glcdDrawPixel(" + i + ", " + jPrev + ", 0x" + prev + ");");
              prev = colHex;
              sameCount = 0;
              jPrev = j;
              iPrev = i;
            }
            else if (j == jPrev + 1){
              sameCount++;
            }
          }
        }
      }
      out.println ("    glcdDrawPixel(" + iPrev + ", " + jPrev + ", 0x" + prev + ");");
      out.close();
    } 
    catch (IOException e){
    }
  }
}