import java.awt.image.*;
import javax.imageio.ImageIO;
import java.io.*;

public class GlcdPicToPix {
  
  public static void main (String [] args){
    try {
      PrintWriter out = new PrintWriter (new FileWriter ("out.txt"));
      BufferedImage bufferedImg = ImageIO.read(new File ("glcdpic3.png"));
      for (int i = 0; i<128; i++){
        for (int j = 0; j<128; j++){
          if (bufferedImg.getRGB(i, j) != -1){
            int temp = bufferedImg.getRGB(i,j);
            temp = temp & 0xFFFFFF;
            String colHex = Integer.toHexString(temp);
            out.println ("    glcdDrawPixel(" + i + ", " + j + ", 0x" + colHex + ");");
          }
        }
      }
      out.close();
    } 
    catch (IOException e){
    }
  }
}