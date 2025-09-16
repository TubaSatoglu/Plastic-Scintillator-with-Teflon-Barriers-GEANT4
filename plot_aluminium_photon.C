#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TSpline.h>
#include <iostream>

void plot_aluminium_photon() { // Updated function name
    // Gamma enerjileri (MeV)
    double energy_levels[] = {0.1, 0.5, 0.7, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Toplam optik fotonlar
    double sum_photons[] = {
        0, 288, 465, 632, 1657, 3167, 3847, 5269, 5731, 6272, 7666, 9284, 10695
    };

    int n = sizeof(energy_levels) / sizeof(energy_levels[0]);

    // Grafik nesnesi oluştur
    TGraph *graph = new TGraph(n, energy_levels, sum_photons);
    graph->SetTitle("Optical Photon Production vs. Gamma Energy;Gamma Energy (MeV);Total Optical Photons");

    // Eğriyi düzgünleştirmek için spline kullan
    TSpline3 *spline = new TSpline3("spline", graph);
    spline->SetLineColor(kBlue);
    spline->SetLineWidth(2);

    // Kanvas oluştur
    TCanvas *c1 = new TCanvas("c1", "Optical Photon Production", 800, 600);
    graph->SetMarkerStyle(21);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kBlue);
    graph->SetLineWidth(2);

    // Grafik çizdir
    graph->Draw("AP");
    spline->Draw("same");

    // Grid ekle
    c1->SetGrid();

    // Açıklama ekle
    TLegend *legend = new TLegend(0.6, 0.2, 0.9, 0.3);
    legend->AddEntry(graph, "Total Optical Photons (Data)", "p");
    legend->AddEntry(spline, "Smoothed Curve (Spline)", "l");
    legend->Draw();

    // Çıktıyı kaydet
    c1->SaveAs("photon_production.png");
}

