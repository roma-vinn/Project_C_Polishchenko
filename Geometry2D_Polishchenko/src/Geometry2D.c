//
//  Geometry2D.c
//  Geometry2D_Polishchenko
//
//  Created by Roman Polishchenko on 5/26/19.
//  Copyright © 2019 Roman Polishchenko. All rights reserved.
//

#include "Geometry2D.h"

// ================= Point2D ================= //

Point2D *createPoint(DTYPE x, DTYPE y) {
    Point2D *point = (Point2D*) malloc(sizeof(Point2D));
    point->x = x;
    point->y = y;
    return point;
}

void freePoint(Point2D *point) {
    free(point);
}

// ================= Segment2D ================= //
// auxilary function to find length of vector made of given points
// consider that point 'a' is start, 'b' - is end
DTYPE _length(Point2D* a, Point2D* b) {
    return sqrt(pow(b->x-a->x, 2) + pow(b->y-a->y, 2));
}

Segment2D *createSegment(Point2D *a, Point2D *b) {
    // allocate memory
    Segment2D *segm = (Segment2D*) malloc(sizeof(Segment2D));
    segm->a = (Point2D*) malloc(sizeof(Point2D));
    segm->b = (Point2D*) malloc(sizeof(Point2D));
    // copy values, not only pointers
    *(segm->a) = *a;
    *(segm->b) = *b;
    segm->length = _length(a, b);
    return segm;
}

void freeSegment(Segment2D *segment) {
    free(segment->a);
    free(segment->b);
    free(segment);
}

// consider that point 'a' is start, 'b' - is end
DTYPE multS(Segment2D *s1, Segment2D *s2) {
    // segment s1 -> vector (x1, y1)
    DTYPE x1 = s1->b->x - s1->a->x;
    DTYPE y1 = s1->b->y - s1->a->y;
    
    // segment s2 -> vector (x2, y2)
    DTYPE x2 = s2->b->x - s2->a->x;
    DTYPE y2 = s2->b->y - s2->a->y;
    
    return x1 * x2 + y1 * y2;
}

// consider that point 'a' is start, 'b' - is end
DTYPE mult(Segment2D *s1, Segment2D *s2) {
    // segment s1 -> vector (x1, y1)
    DTYPE x1 = s1->b->x - s1->a->x;
    DTYPE y1 = s1->b->y - s1->a->y;
    
    // segment s2 -> vector (x2, y2)
    DTYPE x2 = s2->b->x - s2->a->x;
    DTYPE y2 = s2->b->y - s2->a->y;
    
    return x1 * y2 - x2 * y1;
}

// ================= Triangle2D ================= //

Triangle2D *createTriangle(Point2D *a, Point2D *b, Point2D *c) {
    Triangle2D *tr = (Triangle2D*) malloc(sizeof(Triangle2D));
    tr->a = (Point2D*) malloc(sizeof(Point2D));
    tr->b = (Point2D*) malloc(sizeof(Point2D));
    tr->c = (Point2D*) malloc(sizeof(Point2D));
    *(tr->a) = *a;
    *(tr->b) = *b;
    *(tr->c) = *c;
    return tr;
}

void freeTriangle(Triangle2D *t) {
    free(t->a);
    free(t->b);
    free(t->c);
    free(t);
}

DTYPE square(Triangle2D *t) {
    Segment2D *ab = createSegment(t->a, t->b),
    *ac = createSegment(t->a, t->c);
    
    DTYPE square = fabs(0.5 * mult(ab, ac));
    
    freeSegment(ab);
    freeSegment(ac);
    
    return square;
}

DTYPE perimeter(Triangle2D *t) {
    DTYPE ab = _length(t->a, t->b),
    ac = _length(t->a, t->c),
    bc = _length(t->b, t->c);

    return ab + ac + bc;
}

DTYPE incircleRadius(Triangle2D *t) {
    // radius = S / p = 2 * S / P
    // where S - square, P - perimeter, p - half of perimeter
    return 2 * square(t) / perimeter(t);
}

Point2D *incircleCenter(Triangle2D *t) {
    Line2D *ba = bisectorA(t), *bb = bisectorB(t);
    Point2D *p = intersectLL(ba, bb);
    freeLine(ba); freeLine(bb);
    return p;
}

DTYPE excircleRadius(Triangle2D *t) {
    // sides of triangle
    DTYPE c = _length(t->a, t->b),
    b = _length(t->a, t->c),
    a = _length(t->b, t->c);
    
    // half of perimeter
    DTYPE p = perimeter(t) / 2;
    
    return (a * b * c) / (4 * sqrt(p * (p-a) * (p-b) * (p-c)));
}

Point2D *excircleCenter(Triangle2D *t) {
    DTYPE x1 = t->a->x, y1 = t->a->y,
    x2 = t->b->x, y2 = t->b->y,
    x3 = t->c->x, y3 = t->c->y;
    DTYPE x12 = x1 - x2, x23 = x2 - x3, x31 = x3 - x1,
    y12 = y1 - y2, y23 = y2 - y3, y31 = y3 - y1;
    DTYPE z1 = x1 * x1 + y1 * y1,
    z2 = x2 * x2 + y2 * y2,
    z3 = x3 * x3 + y3 * y3;
    DTYPE zx = y12 * z3 + y23 * z1 + y31 * z2,
    zy = x12 * z3 + x23 * z1 + x31 * z2,
    z = x12 * y31 - y12 * x31;
    return createPoint(-zx/(2*z), zy/(2*z));
}

Point2D *massCenter(Triangle2D *t) {
    return createPoint((t->a->x + t->b->x + t->c->x)/3,
                       (t->a->y + t->b->y + t->c->y)/3);
}

DTYPE angleA(Triangle2D *t) {
    // sides of triangle
    Segment2D *ab = createSegment(t->a, t->b),
    *ac = createSegment(t->a, t->c);
    
    DTYPE angle = acos( multS(ab, ac) / (ab->length * ac->length) );
    
    freeSegment(ab);
    freeSegment(ac);
    
    return angle;
}

DTYPE angleB(Triangle2D *t) {
    // sides of triangle
    Segment2D *ba = createSegment(t->b, t->a),
    *bc = createSegment(t->b, t->c);
    
    DTYPE angle = acos( multS(ba, bc) / (ba->length * bc->length) );
    
    freeSegment(ba);
    freeSegment(bc);
    
    return angle;
}

DTYPE angleC(Triangle2D *t) {
    // sides of triangle
    Segment2D *cb = createSegment(t->c, t->b),
    *ca = createSegment(t->c, t->a);
    
    DTYPE angle = acos( multS(cb, ca) / (cb->length * ca->length) );
    
    freeSegment(cb);
    freeSegment(ca);
    
    return angle;
}
// ================= Line2D ================= //

Line2D *createLine(Point2D *a, Point2D *b) {
    Line2D *line = (Line2D*) malloc(sizeof(Line2D));
    line->a = a->y - b->y;
    line->b = b->x - a->x;
    line->c = - line->a * a->x - line->b * a->y;
    
    return line;
}

void freeLine(Line2D *line) {
    free(line);
}

ITYPE _pointSign(Line2D *line, Point2D *p) {
    return SIGN(line->a * p->x + line->b * p->y + line->c);
}

// auxilary function for searching determinant
DTYPE _det(DTYPE a11, DTYPE a12, DTYPE a21, DTYPE a22) {
    return a11 * a22 - a12 * a21;
}

Point2D *intersectLL(Line2D *p1, Line2D *p2) {
    // { a1*x + b1*y = -c1,
    // { a2*x + b2*y = -c2, - linear system
    // if det == 0 --> either parallel or equivalent
    if (PD_EQL(_det(p1->a, p1->b, p2->a, p2->b), 0)) {
        // equivalent
        if ( PD_EQL(_det(p1->a, p1->c, p2->a, p2->c), 0)
            && PD_EQL(_det(p1->b, p1->c, p2->b, p2->c), 0) ) {
            return createPoint(INF, INF);
        } else {
            // parallel
            return createPoint(INF, -INF);
        }
    } else {
        // intersection exists
        DTYPE x = - _det(p1->c, p1->b, p2->c, p2->b) / _det(p1->a, p1->b, p2->a, p2->b);
        DTYPE y = - _det(p1->a, p1->c, p2->a, p2->c) / _det(p1->a, p1->b, p2->a, p2->b);
        return createPoint(x, y);
    }
}

Point2D *intersectLS(Line2D *p1, Segment2D *p2) {
    // if both ends of segment are on the line - infinite number of dots
    if (_pointSign(p1, p2->a) == 0 && _pointSign(p1, p2->b) == 0) {
        return createPoint(INF, INF);
    }
    
    // if one of the end of segment is on the line - return it
    if (_pointSign(p1, p2->a) == 0) {
        return createPoint(p2->a->x, p2->a->y);
    }
    if (_pointSign(p1, p2->b) == 0) {
        return createPoint(p2->b->x, p2->b->y);
    }
    
    // if ends of segment are on different sides of the line
    if (_pointSign(p1, p2->a) != _pointSign(p1, p2->b)) {
        Line2D *tmp = createLine(p2->a, p2->b);
        Point2D *inter = intersectLL(p1, tmp);
        
        freeLine(tmp);
        
        return inter;
        
    } else {
        return createPoint(INF, -INF);
    }
}

// auxilary function to check if dot is on the segment
ITYPE _onSegment(Point2D *p1, Segment2D *p2) {
    Segment2D *seg_a = createSegment(p1, p2->a),
    *seg_b = createSegment(p1, p2->b);
    if (PD_EQL(seg_a->length + seg_b->length, p2->length)) {
        freeSegment(seg_a); freeSegment(seg_b);
        return 1;
    } else {
        freeSegment(seg_a); freeSegment(seg_b);
        return 0;
    }
}

Point2D *intersectSS(Segment2D *p1, Segment2D *p2) {
    // point of intersection of two lines build on given segments
    Line2D *line1 = createLine(p1->a, p1->b),
    *line2 = createLine(p2->a, p2->b);
    
    Point2D *p = intersectLL(line1, line2);
    
    freeLine(line1); freeLine(line2);
    
    // if it's the only point
    if (p->x != INF) {
        // and if it belongs to both segments
        if (_onSegment(p, p1) && _onSegment(p, p2)) {
            return p;
        } else {
            // else - there isn't intersection
            freePoint(p);
            return createPoint(INF, -INF);
        }
    } else {
        // if lines are parallel --> no common points
        if (p->y == -INF) {
            freePoint(p);
            return createPoint(INF, -INF);
        } else {  // else if lines are equivalent
            // if all of the ends of any segment don't belong to the other one
            // --> no common points
            if (!_onSegment(p1->a, p2) && !_onSegment(p1->b, p2)
                && !_onSegment(p2->a, p1) && !_onSegment(p2->b, p1)) {
                freePoint(p);
                return createPoint(INF, -INF);
            } else {
                Segment2D *segm1 = createSegment(p1->a, p2->a),
                *segm2 = createSegment(p1->a, p2->b),
                *segm3 = createSegment(p1->b, p2->a),
                *segm4 = createSegment(p1->b, p2->b);
                DTYPE max_len = MAX(MAX(segm1->length, segm2->length),
                                    MAX(segm3->length, segm4->length));
                // if sum of lengths == max len --> lay consistently on the line
                // --> one common point
                if (PD_EQL(p1->length + p2->length, max_len)) {
                    if (PD_EQL(segm1->length, 0) || PD_EQL(segm2->length, 0)) {
                        freeSegment(segm1); freeSegment(segm2);
                        freeSegment(segm3); freeSegment(segm4);
                        freePoint(p);
                        return createPoint(p1->a->x, p1->a->y);
                    } else {
                        freeSegment(segm1); freeSegment(segm2);
                        freeSegment(segm3); freeSegment(segm4);
                        freePoint(p);
                        return createPoint(p1->b->x, p1->b->y);
                    }
                } else {
                    // have common part --> infinite number of common points
                    freeSegment(segm1); freeSegment(segm2);
                    freeSegment(segm3); freeSegment(segm4);
                    freePoint(p);
                    return createPoint(INF, INF);
                }
            }
        }
    }
}

Line2D *medianA(Triangle2D *t) {
    Point2D *tmp = createPoint((t->b->x + t->c->x)/2, (t->b->y + t->c->y)/2);
    Line2D *line = createLine(t->a, tmp);
    freePoint(tmp);
    return line;
}

Line2D *medianB(Triangle2D *t) {
    Point2D *tmp = createPoint((t->a->x + t->c->x)/2, (t->a->y + t->c->y)/2);
    Line2D *line = createLine(t->b, tmp);
    freePoint(tmp);
    return line;
}

Line2D *medianC(Triangle2D *t) {
    Point2D *tmp = createPoint((t->b->x + t->a->x)/2, (t->b->y + t->a->y)/2);
    Line2D *line = createLine(t->c, tmp);
    freePoint(tmp);
    return line;
}

Line2D *bisectorA(Triangle2D *t) {
    DTYPE ab_x = t->b->x - t->a->x,
    ab_y = t->b->y - t->a->y,
    ac_x = t->c->x - t->a->x,
    ac_y = t->c->y - t->a->y;
    DTYPE ab_len = sqrt(pow(ab_x, 2) + pow(ab_y, 2)),
    ac_len = sqrt(pow(ac_x, 2) + pow(ac_y, 2));
    ab_x /= ab_len;
    ab_y /= ab_len;
    ac_x /= ac_len;
    ac_y /= ac_len;
    DTYPE bisect_x = ab_x + ac_x,
    bisect_y = ab_y + ac_y;
    Point2D *tmp = createPoint(t->a->x + bisect_x, t->a->y + bisect_y);
    Line2D *line = createLine(t->a, tmp);
    freePoint(tmp);
    return line;
}

Line2D *bisectorB(Triangle2D *t) {
    DTYPE ba_x = t->a->x - t->b->x,
    ba_y = t->a->y - t->b->y,
    bc_x = t->c->x - t->b->x,
    bc_y = t->c->y - t->b->y;
    DTYPE ba_len = sqrt(pow(ba_x, 2) + pow(ba_y, 2)),
    bc_len = sqrt(pow(bc_x, 2) + pow(bc_y, 2));
    ba_x /= ba_len;
    ba_y /= ba_len;
    bc_x /= bc_len;
    bc_y /= bc_len;
    DTYPE bisect_x = ba_x + bc_x,
    bisect_y = ba_y + bc_y;
    Point2D *tmp = createPoint(t->b->x + bisect_x, t->b->y + bisect_y);
    Line2D *line = createLine(t->b, tmp);
    freePoint(tmp);
    return line;
}

Line2D *bisectorC(Triangle2D *t) {
    DTYPE cb_x = t->b->x - t->c->x,
    cb_y = t->b->y - t->c->y,
    ca_x = t->a->x - t->c->x,
    ca_y = t->a->y - t->c->y;
    DTYPE cb_len = sqrt(pow(cb_x, 2) + pow(cb_y, 2)),
    ca_len = sqrt(pow(ca_x, 2) + pow(ca_y, 2));
    cb_x /= cb_len;
    cb_y /= cb_len;
    ca_x /= ca_len;
    ca_y /= ca_len;
    DTYPE bisect_x = cb_x + ca_x,
    bisect_y = cb_y + ca_y;
    Point2D *tmp = createPoint(t->c->x + bisect_x, t->c->y + bisect_y);
    Line2D *line = createLine(t->c, tmp);
    freePoint(tmp);
    return line;
}

Line2D *altitudeA(Triangle2D *t) {
    Line2D *op_side = createLine(t->b, t->c);
    DTYPE a = op_side->a, b = op_side->b;
    freeLine(op_side);
    Point2D *tmp = createPoint(t->a->x + a, t->a->y + b);
    Line2D *line = createLine(t->a, tmp);
    freePoint(tmp);
    return line;
}

Line2D *altitudeB(Triangle2D *t) {
    Line2D *op_side = createLine(t->a, t->c);
    DTYPE a = op_side->a, b = op_side->b;
    freeLine(op_side);
    Point2D *tmp = createPoint(t->b->x + a, t->b->y + b);
    Line2D *line = createLine(t->b, tmp);
    freePoint(tmp);
    return line;
}

Line2D *altitudeC(Triangle2D *t) {
    Line2D *op_side = createLine(t->b, t->a);
    DTYPE a = op_side->a, b = op_side->b;
    freeLine(op_side);
    Point2D *tmp = createPoint(t->c->x + a, t->c->y + b);
    Line2D *line = createLine(t->c, tmp);
    freePoint(tmp);
    return line;
}

ITYPE figuresSquares(Triangle2D *triangle, Line2D *line, DTYPE *s1, DTYPE *s2) {
    Segment2D *ab = createSegment(triangle->a, triangle->b),
    *bc = createSegment(triangle->b, triangle->c),
    *ca = createSegment(triangle->c, triangle->a);
    
    Point2D *ip1 = intersectLS(line, ab),
    *ip2 = intersectLS(line, bc),
    *ip3 = intersectLS(line, ca);
    
    freeSegment(ab); freeSegment(bc); freeSegment(ca);
    
    // if line doesn't intersect our triangle at all
    if ( ip1->x == INF && ip2->x == INF && ip3->x == INF) {
        *s1 = -1;
        *s2 = -1;
        freePoint(ip1); freePoint(ip2); freePoint(ip3);
        return 1;
    }
    
    // if line intersect each side of triangle in one point
    // --> devides it exactly on two new triangles
    if ( ip1->x != INF && ip2->x != INF && ip3->x != INF) {
        // one of this triangles definitely has non-0 square --> s1
        Triangle2D *t1 = createTriangle(triangle->a, ip1, ip2),
        *t2 = createTriangle(triangle->a, ip2, ip3),
        *t3 = createTriangle(triangle->b, ip1, ip2),
        *t4 = createTriangle(triangle->b, ip2, ip3);
        
        *s1 = MAX(MAX(square(t1), square(t2)), MAX(square(t3), square(t4)));
        freeTriangle(t1); freeTriangle(t2); freeTriangle(t3); freeTriangle(t4);
        freePoint(ip1); freePoint(ip2); freePoint(ip3);
        DTYPE s_tr = square(triangle);
        *s2 = s_tr - *s1;
        return 0;
    }

    // only one case left - when line intersects only 2 sides
    
    // if it's not ab --> line has common points with ac and bc
    if (ip1->x == INF) {
        Triangle2D *tmp = createTriangle(triangle->c, ip2, ip3);
        *s1 = square(tmp);
        freeTriangle(tmp);
        *s2 = square(triangle) - *s1;
        if (PD_EQL(MIN(*s1, *s2), 0)) {
            *s1 = -1;
            *s2 = -1;
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 1;
        } else {
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 0;
        }
    }
    
    // if it's not bc --> line has common points with ac and ab
    if (ip2->x == INF) {
        Triangle2D *tmp = createTriangle(triangle->a, ip1, ip3);
        *s1 = square(tmp);
        freeTriangle(tmp);
        *s2 = square(triangle) - *s1;
        if (PD_EQL(MIN(*s1, *s2), 0)) {
            *s1 = -1;
            *s2 = -1;
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 1;
        } else {
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 0;
        }
    }
    
    // if it's not ac --> line has common points with bc and ab
    if (ip3->x == INF) {
        Triangle2D *tmp = createTriangle(triangle->b, ip1, ip2);
        *s1 = square(tmp);
        freeTriangle(tmp);
        *s2 = square(triangle) - *s1;
        if (PD_EQL(MIN(*s1, *s2), 0)) {
            *s1 = -1;
            *s2 = -1;
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 1;
        } else {
            freePoint(ip1); freePoint(ip2); freePoint(ip3);
            return 0;
        }
    }
    
    // smth went wrong
    *s1 = -1;
    *s2 = -1;
    freePoint(ip1); freePoint(ip2); freePoint(ip3);
    return 1;
}
