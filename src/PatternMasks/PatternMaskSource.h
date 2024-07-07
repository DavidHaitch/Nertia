#ifndef PATTERNMASKSOURCE
#define PATTERNMASKSOURCE
class PatternMaskSource
{
public:
    PatternMaskSource()
    {
    }

    virtual void setMirror(bool _mirror)
    {
        mirror = _mirror;
    }

    virtual int get(int pixel)
    {
        return mirror && (pixel / getAnimationResolution()) % 2 != 1 ? getMask(frameCounter, pixel) : getMask(frameCounter, getAnimationResolution() - pixel);
    }

    virtual void step()
    {
        frameCounter++;
        frameCounter %= getAnimationFrames();
    }

    ~PatternMaskSource()
    {
    }

    protected:
    virtual int getMask(int frame, int pixel) = 0;
    int frameCounter = 0;
    virtual int getAnimationFrames() = 0;
    virtual int getAnimationResolution() = 0;
    bool mirror = false;
};
#endif