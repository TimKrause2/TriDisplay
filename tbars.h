#include <GLES3/gl31.h>

#define N_BARS               60

class TBars
{
private:
    float data[N_BARS];
    int i_current;
    GLuint program;
    GLuint vboIds[2];
    GLuint vaoId;
public:
    TBars(void);
    ~TBars(void);
    bool Init(void);
    void Draw(int width, int height);
    void Update(float x);
};

