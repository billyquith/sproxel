#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "Global.h"
#include "UndoManager.h"

#include <ImathVec.h>
#include <ImathColor.h>

////////////////////////////////////////////////////////////////////////////////
// Abstract base class for Sproxel tools and their execution states
////////////////////////////////////////////////////////////////////////////////
class ToolState
{
public:
    ToolState(UndoManager* um) : m_clicksRemain(0),
                                 p_undoManager(um),
                                 m_ray(Imath::Line3d()),
                                 m_color(Imath::Color4f(0.0f, 0.0f, 0.0f, 0.0f)),
                                 m_index(0),
                                 p_gvg(NULL),
                                 m_supportsDrag(false) {}

    virtual ~ToolState() {}

    virtual void execute() = 0;
    virtual SproxelTool type() = 0;
    virtual std::vector<Imath::V3i> voxelsAffected() = 0;

    virtual void executeErase() {}

    const Imath::Line3d& ray() const { return m_ray; }

    void set(VoxelGridGroupPtr gvg, const Imath::Box3i &edit_bounds,
      const Imath::Line3d& ray, const Imath::Color4f& color, int index)
    {
        p_gvg = gvg;
        m_editBounds=edit_bounds;
        m_ray = ray;
        m_color = color;
        m_index = index;
    }

    int clicksRemaining() { return m_clicksRemain; }
    void decrementClicks()
    {
        m_clicksRemain--;
        if (m_clicksRemain == 0)
            m_clicksRemain = m_totalClicks;
    }

    virtual void setDragSupport(bool support) { m_supportsDrag = support; }
    virtual bool supportsDrag() { return m_supportsDrag; }

protected:
    int m_totalClicks;
    int m_clicksRemain;

    UndoManager* p_undoManager;
    Imath::Line3d m_ray;
    Imath::Box3i m_editBounds;
    Imath::Color4f m_color;
    int m_index;
    VoxelGridGroupPtr p_gvg;
    bool m_supportsDrag;


    std::vector<Imath::V3i> rayIntersection(const Imath::Line3d &worldRay)
    {
      Imath::Line3d localRay = worldRay * p_gvg->transform().inverse();
      return walk_ray(localRay, m_editBounds);
    }
};


////////////////////////////////////////////////////////////////////////////////
// Splat tool
////////////////////////////////////////////////////////////////////////////////
class SplatToolState : public ToolState
{
public:
    SplatToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }
    ~SplatToolState() {}

    void execute();
    SproxelTool type() { return TOOL_SPLAT; }
    std::vector<Imath::V3i> voxelsAffected();
};


////////////////////////////////////////////////////////////////////////////////
// Flood tool
////////////////////////////////////////////////////////////////////////////////
class FloodToolState : public ToolState
{
public:
    FloodToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }
    ~FloodToolState() {}

    void execute();
    SproxelTool type() { return TOOL_FLOOD; }
    std::vector<Imath::V3i> voxelsAffected();

private:
    void setNeighborsRecurse(const Imath::V3i& alreadySet,
                             const Imath::Color4f& repColor,
                             const Imath::Color4f& newColor,
                             int newIndex);
};


////////////////////////////////////////////////////////////////////////////////
// Eraser tool
////////////////////////////////////////////////////////////////////////////////
class EraserToolState : public ToolState
{
public:
    EraserToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }
    ~EraserToolState() {}

    void execute();
    SproxelTool type() { return TOOL_ERASER; }
    std::vector<Imath::V3i> voxelsAffected();
};


////////////////////////////////////////////////////////////////////////////////
// Replace tool
////////////////////////////////////////////////////////////////////////////////
class ReplaceToolState : public ToolState
{
public:
    ReplaceToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }

    ~ReplaceToolState() {}

    void execute();
    SproxelTool type() { return TOOL_REPLACE; }
    std::vector<Imath::V3i> voxelsAffected();
};


////////////////////////////////////////////////////////////////////////////////
// Ray tool
////////////////////////////////////////////////////////////////////////////////
class RayToolState : public ToolState
{
public:
    RayToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }
    ~RayToolState() {}

    void execute();
    SproxelTool type() { return TOOL_RAY; }
    std::vector<Imath::V3i> voxelsAffected();
};


////////////////////////////////////////////////////////////////////////////////
// Slab tool
////////////////////////////////////////////////////////////////////////////////
class SlabToolState : public ToolState
{
public:
    SlabToolState(UndoManager* um) : ToolState(um)
    {
        m_workingAxis = Y_AXIS;
        m_totalClicks = m_clicksRemain = 1;
    }
    ~SlabToolState() {}

    virtual void setAxis(SproxelAxis axis)
    {
        m_workingAxis = axis;
    }

    void execute();
    SproxelTool type() { return TOOL_SLAB; }
    std::vector<Imath::V3i> voxelsAffected();

private:
    SproxelAxis m_workingAxis;
};


////////////////////////////////////////////////////////////////////////////////
// Line tool
////////////////////////////////////////////////////////////////////////////////
class LineToolState : public ToolState
{
public:
    LineToolState(UndoManager* um) : ToolState(um), m_startPoint(-1, -1, -1)
    {
        m_totalClicks = m_clicksRemain = 2;
    }
    ~LineToolState() {}

    void execute();
    SproxelTool type() { return TOOL_LINE; }
    std::vector<Imath::V3i> voxelsAffected();

private:
    Imath::V3i m_startPoint;
};


////////////////////////////////////////////////////////////////////////////////
// Box tool
////////////////////////////////////////////////////////////////////////////////
class BoxToolState : public ToolState
{
public:
    BoxToolState(UndoManager* um) : ToolState(um), m_startPoint(-1, -1, -1)
    {
        m_totalClicks = m_clicksRemain = 2;
    }
    ~BoxToolState() {}

    void execute();
    SproxelTool type() { return TOOL_BOX; }
    std::vector<Imath::V3i> voxelsAffected();

private:
    Imath::V3i m_startPoint;
};


////////////////////////////////////////////////////////////////////////////////
// Extrude tool
////////////////////////////////////////////////////////////////////////////////
class ExtrudeToolState : public ToolState
{
public:
    ExtrudeToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }
    ~ExtrudeToolState() {}

    void execute();
    void executeErase();
    SproxelTool type() { return TOOL_EXTRUDE; }
    std::vector<Imath::V3i> voxelsAffected();

private:
    Imath::V3i m_dir;

    void doExtrude(bool is_erase);

    void fillExtrudeMap(GameVoxelGrid<char> &map, const Imath::V3i &mapOfs,
      const Imath::V3i &pos, int axis);
};


////////////////////////////////////////////////////////////////////////////////
// Dropper tool
////////////////////////////////////////////////////////////////////////////////
class DropperToolState : public ToolState
{
public:
    DropperToolState(UndoManager* um) : ToolState(um)
    {
        m_totalClicks = m_clicksRemain = 1;
    }

    ~DropperToolState() {}

    void execute();
    SproxelTool type() { return TOOL_DROPPER; }
    std::vector<Imath::V3i> voxelsAffected();
};


#endif
