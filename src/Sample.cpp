#include "Sample.h"


// sample tracking and allele sorting
// the number of observations for this allele
int Sample::observationCount(Allele& allele) {
    return observationCount(allele.currentBase);
}

int Sample::observationCountInclPartials(void) {
    return observationCount() + partialObservationCount();
}

double Sample::observationCountInclPartials(Allele& allele) {
    return observationCountInclPartials(allele.currentBase);
}

double Sample::partialObservationCount(Allele& allele) {
    return partialObservationCount(allele.currentBase);
}

// the number of observations for this base
int Sample::observationCount(const string& base) {
    Sample::iterator g = find(base);
    if (g != end())
        return g->second.size();
    else
        return 0;
}

int Sample::partialObservationCount(void) {
    return reversePartials.size();
}

double Sample::partialObservationCount(const string& base) {
    double scaledPartialCount = 0;
    map<string, vector<Allele*> >::iterator g = partials.find(base);
    if (g != partials.end()) {
        vector<Allele*>& supportingObs = g->second;
        for (vector<Allele*>::iterator a = supportingObs.begin(); a != supportingObs.end(); ++a) {
            scaledPartialCount += (double) 1 / (double) reversePartials[*a].size();
        }
    }
    return scaledPartialCount;
}

double Sample::observationCountInclPartials(const string& base) {
    return observationCount(base) + partialObservationCount(base);
}

// the total number of observations
int Sample::observationCount(void) {
    int count = 0;
    for (Sample::iterator g = begin(); g != end(); ++g) {
        count += g->second.size();
    }
    return count;
}

int Sample::qualSum(Allele& allele) {
    return qualSum(allele.currentBase);
}

int Sample::qualSum(const string& base) {
    Sample::iterator g = find(base);
    int qsum = 0;
    if (g != end()) {
        vector<Allele*>& alleles = g->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            qsum += (*a)->quality;
        }
    }
    return qsum;
}

double Sample::partialQualSum(Allele& allele) {
    return partialQualSum(allele.currentBase);
}

double Sample::partialQualSum(const string& base) {
    Sample::iterator g = partials.find(base);
    double qsum = 0;
    if (g != partials.end()) {
        vector<Allele*>& alleles = g->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            qsum += (double) (*a)->quality / (double) reversePartials[*a].size();
        }
    }
    return qsum;
}

// sample tracking and allele sorting
// the number of observations for this allele
int Samples::observationCount(Allele& allele) {
    return observationCount(allele.currentBase);
}

double Samples::observationCountInclPartials(Allele& allele) {
    return observationCountInclPartials(allele.currentBase);
}

double Samples::partialObservationCount(Allele& allele) {
    return partialObservationCount(allele.currentBase);
}

// the number of observations for this base
int Samples::observationCount(const string& base) {
    int c = 0;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        c += s->second.observationCount(base);
    }
    return c;
}

double Samples::partialObservationCount(const string& base) {
    double c;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        c += s->second.partialObservationCount(base);
    }
    return c;
}

double Samples::observationCountInclPartials(const string& base) {
    return observationCount(base) + partialObservationCount(base);
}

// the total number of observations
int Samples::observationCount(void) {
    int c = 0;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        c += s->second.observationCount();
    }
    return c;
}

double Samples::observationCountInclPartials(void) {
    double c = 0;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        c += s->second.observationCountInclPartials();
    }
    return c;
}

int Samples::qualSum(Allele& allele) {
    qualSum(allele.currentBase);
}

int Samples::qualSum(const string& base) {
    int q = 0;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        q += s->second.qualSum(base);
    }
    return q;
}

double Samples::partialQualSum(Allele& allele) {
    partialQualSum(allele.currentBase);
}

double Samples::partialQualSum(const string& base) {
    double q = 0;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        q += s->second.partialQualSum(base);
    }
    return q;
}

map<string, double> Samples::estimatedAlleleFrequencies(void) {
    map<string, long double> qualsums;
    for (Samples::iterator s = begin(); s != end(); ++s) {
        Sample& sample = s->second;
        for (Sample::iterator o = sample.begin(); o != sample.end(); ++o) {
            const string& base = o->first;
            qualsums[base] += sample.qualSum(base);
        }
    }
    long double total = 0;
    for (map<string, long double>::iterator q = qualsums.begin(); q != qualsums.end(); ++q) {
        total += q->second;
    }
    map<string, double> freqs;
    for (map<string, long double>::iterator q = qualsums.begin(); q != qualsums.end(); ++q) {
        freqs[q->first] = q->second / total;
        //cerr << "estimated frequency " << q->first << " " << freqs[q->first] << endl;
    }
    return freqs;
}

// puts alleles into the right bins if they have changed their base (as
// occurs in the case of reference alleles)
void Sample::sortReferenceAlleles(void) {
    for (Sample::iterator g = begin(); g != end(); ++g) {
        const string& groupBase = g->first;
        vector<Allele*>& alleles = g->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            const string& base = (*a)->currentBase;
            if (base != groupBase) {
                Sample::iterator g = find(base);
                if (g != end()) {
                    g->second.push_back(*a);
                } else {
                    vector<Allele*> alleles;
                    alleles.push_back(*a);
                    insert(begin(), make_pair(base, alleles));
                }
                *a = NULL;
            }
        }
        alleles.erase(remove(alleles.begin(), alleles.end(), (Allele*)NULL), alleles.end());
    }
}

pair<pair<int, int>, pair<int, int> >
Sample::baseCount(string refbase, string altbase) {

    int forwardRef = 0;
    int reverseRef = 0;
    int forwardAlt = 0;
    int reverseAlt = 0;

    for (Sample::iterator s = begin(); s != end(); ++s) {
        vector<Allele*>& alleles = s->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            string base = (*a)->currentBase;
            AlleleStrand strand = (*a)->strand;
            if (base == refbase) {
                if (strand == STRAND_FORWARD)
                    ++forwardRef;
                else if (strand == STRAND_REVERSE)
                    ++reverseRef;
            } else if (base == altbase) {
                if (strand == STRAND_FORWARD)
                    ++forwardAlt;
                else if (strand == STRAND_REVERSE)
                    ++reverseAlt;
            }
        }
    }

    return make_pair(make_pair(forwardRef, forwardAlt), make_pair(reverseRef, reverseAlt));

}

int Sample::baseCount(string base, AlleleStrand strand) {

    int count = 0;
    for (Sample::iterator g = begin(); g != end(); ++g) {
        vector<Allele*>& alleles = g->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            if ((*a)->currentBase == base && (*a)->strand == strand)
                ++count;
        }
    }
    return count;

}


string Sample::json(void) {
    stringstream out;
    out << "[";
    bool first = true;
    for (map<string, vector<Allele*> >::iterator g = this->begin(); g != this->end(); ++g) {
        vector<Allele*>& alleles = g->second;
        for (vector<Allele*>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
            if (!first) { out << ","; } else { first = false; }
            out << (*a)->json();
        }
    }
    out << "]";
    return out.str();
}

void groupAlleles(Samples& samples, map<string, vector<Allele*> >& alleleGroups) {
    for (Samples::iterator s = samples.begin(); s != samples.end(); ++s) {
        Sample& sample = s->second;
        for (Sample::iterator g = sample.begin(); g != sample.end(); ++g) {
            const string& base = g->first;
            const vector<Allele*>& alleles = g->second;
            vector<Allele*>& group = alleleGroups[base];
            group.reserve(group.size() + distance(alleles.begin(), alleles.end()));
            group.insert(group.end(), alleles.begin(), alleles.end());
        }
    }
}

bool sufficientAlternateObservations(Samples& samples, int mincount, float minfraction) {

    int totalAlternateCount = 0;
    int totalReferenceCount = 0;

    for (Samples::iterator s = samples.begin(); s != samples.end(); ++s) {

        //cerr << s->first << endl;
        Sample& sample = s->second;
        int alternateCount = 0;
        int observationCount = 0;

        for (Sample::iterator group = sample.begin(); group != sample.end(); ++group) {
            const string& base = group->first;
            //cerr << base << endl;
            vector<Allele*>& alleles = group->second;
            //cerr << alleles.size() << endl;
            if (alleles.size() == 0)
                continue;
            if (alleles.front()->type != ALLELE_REFERENCE) {
                alternateCount += alleles.size();
            } else {
                totalReferenceCount += alleles.size();
            }
            observationCount += alleles.size();
        }

        if (alternateCount >= mincount && ((float) alternateCount / (float) observationCount) >= minfraction)
            return true;
        totalAlternateCount += alternateCount;
    
    }

    // always analyze if we have more alternate observations than reference observations
    // this is meant to catch the case in which the reference is the rare allele
    // it will probably also catch cases in which we have very low coverage
    if (totalReferenceCount < totalAlternateCount) {
        return true;
    }

    return false;

}


int countAlleles(Samples& samples) {

    int count = 0;
    for (Samples::iterator s = samples.begin(); s != samples.end(); ++s) {
        Sample& sample = s->second;
        for (Sample::iterator sg = sample.begin(); sg != sample.end(); ++sg) {
            count += sg->second.size();
        }
    }
    return count;

}

ostream& operator<<(ostream& out, Sample& sample) {
    for (Sample::iterator s = sample.begin(); s != sample.end(); ++s) {
        out << s->first << ":" << s->second.size() << " ";
    }
    return out;
}


void Samples::assignPartialSupport(vector<Allele>& alleles,
                                   vector<Allele*>& partialObservations,
                                   map<string, vector<Allele*> >& partialObservationGroups,
                                   map<Allele*, set<Allele*> >& partialObservationSupport) {

    //map<string, vector<Allele*> >& partials,
    //map<Allele*, vector<string*> >& reversePartials);

    // the canonical alleles as supported by partials
    //map<string, vector<Allele*> > supportingObs;
    // the partials as they support canonical alleles
    //map<Allele*, set<Allele*> > supportedAlleles;

    for (vector<Allele>::iterator a = alleles.begin(); a != alleles.end(); ++a) {
        Allele& allele = *a;
        //string& base = allele.currentBase;
        // hacks here
        string& aseq = allele.alternateSequence;
        // construct pseudo-sequence
        for (vector<Allele*>::iterator p = partialObservations.begin(); p != partialObservations.end(); ++p) {
            Allele& partial = **p;
            string& pseq = partial.alternateSequence;
            bool same = false;
            if (aseq.size() >= pseq.size()
                && (aseq.substr(0, pseq.size()) == pseq
                    || aseq.substr(aseq.size()-pseq.size()) == pseq)) {
                // dAY's du saem
                partialObservationGroups[allele.currentBase].push_back(*p);
                partialObservationSupport[*p].insert(&*a);
                //cerr << "partial support ... " << *p << " for " << &*a << endl;
                same = true;
            }
            //cerr << "pseudos\t" << allele << endl 
            //     << "pseudos\t" << partial << endl;
            //cerr << "pseudo_alt\t" << string(max(0, (int) (allele.position - partial.position)), ' ') << allele.alternateSequence << "\t" << (same ? "==" : "!=" ) << endl;
            //cerr << "pseudo_obs\t" << string(max(0, (int) (partial.position - allele.position)), ' ') << partial.alternateSequence << endl;
        }
    }

    for (vector<Allele*>::iterator p = partialObservations.begin(); p != partialObservations.end(); ++p) {
        // get the sample
        Allele& partial = **p;
        Samples::iterator siter = find(partial.sampleID);
        if (siter == end()) {
            continue;
        }
        Sample& sample = siter->second;
        set<Allele*>& supported = partialObservationSupport[*p];
        for (set<Allele*>::iterator s = supported.begin(); s != supported.end(); ++s) {
            sample.partials[(*s)->currentBase].push_back(*p);
        }
        sample.reversePartials[*p] = supported;
    }

}

bool Sample::observationSupports(Allele* obs, Allele* allele) {
    if (obs->currentBase == allele->currentBase) {
        return true;
    } else {
        map<Allele*, set<Allele*> >::iterator p = reversePartials.find(obs);
        if (p != reversePartials.end()) {
            set<Allele*>& supports = p->second;
            if (supports.find(allele) != supports.end()) {
                return true;
            }
        }
        return false;
    }
}
